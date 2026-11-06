#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>

//#include "../generic_fpga.h"
//#include "generic.h"
//#include "coines.h"
//#include "horizon_addrmap.h"

// #define INFILE 1
#define DIRECT_CASE 1

// ==========================================Generic_fpga.c==========================================================
// If M_PI is not defined, we can define it manually
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Struct for heater profile data points(step and value)
typedef struct {
    uint8_t step;
    float value;
	uint8_t state;
} HeaterProfilePoint;


// --- Aux Intern Functions (not exposed in .h)

// Helper function to dynamically add a point to an array
// Returns the new size of the array
static int addPoint(HeaterProfilePoint **arr, int *current_size, int *current_capacity, int step, float value, int state)
{
	if (*current_size >= *current_capacity)
	{
		*current_capacity *= 2; // Double the capacity
		if (*current_capacity == 0)
			*current_capacity = 10; // Initial capacity if it was 0

		// printf("DEBUG: Reallocating memory. Old capacity: %d, New capacity: %d, Current size: %d\n", *current_capacity / 2, *current_capacity, *current_size);

		*arr = (HeaterProfilePoint *)realloc(*arr, (*current_capacity) * sizeof(HeaterProfilePoint));
		if (*arr == NULL)
		{
			perror("Failed to reallocate memory for signal points");
			// exit(EXIT_FAILURE); //In a real librery, would return an error.
		}
	}
	// Debugging point addition
	// printf("DEBUG: Adding point: step=%d, value=%.2f, size=%d, capacity=%d\n", step, value, *current_size, *current_capacity);

	(*arr)[*current_size].step = step;
	(*arr)[*current_size].value = value;
	(*arr)[*current_size].state = state;
	(*current_size)++;
	return *current_size;
}

// Generates an upward ramp segment
// Returns the number of points generated for this segment
static int generateUpwardRampSegment(uint8_t htr_mode, uint16_t htr_max_pwr, float offset, float dt_step, int n_step, int start_global_step, HeaterProfilePoint **segment_data, int *current_size, int *current_capacity)
{
	float current_value = 0.0;
	int current_global_step = start_global_step;
	int increments_made = 0;
	int points_generated = 0;

	if (dt_step == 0 || offset == 0 || n_step == 0)
	{
		// If no ramp is needed (dt_step=0 or offset=0), just add the starting point at 0.0
		addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 0);
		return points_generated;
	}

	int total_increments = (int)(offset / dt_step);
	if (offset > 0 && total_increments == 0)
	{ // Ensure at least one increment if offset > 0 and dt_step is large
		total_increments = 1;
		increments_made++;
	}

	addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 0);
	points_generated++;
	current_global_step++;

	if (n_step > 1)
	{
		current_value += dt_step;
		if (htr_mode != 1)
		{ // if htr_mode is not OL (without mon)
			if (current_value > htr_max_pwr)
				current_value = htr_max_pwr;
		}
		addPoint(segment_data, current_size, current_capacity, current_global_step, current_value, 0);
		points_generated++;
		// current_global_step++;
	}
	// if (n_step != 0) {
	while (increments_made <= total_increments)
	{
		if ((current_global_step - start_global_step) % n_step == 0)
		{
			current_value += dt_step;
			increments_made++;
		}
		if (current_value > offset)
		{
			current_value = offset;
		}

		if (htr_mode != 1)
		{ // if htr_mode is not OL (without mon)
			if (current_value > htr_max_pwr)
				current_value = htr_max_pwr;
		}
		addPoint(segment_data, current_size, current_capacity, current_global_step, current_value, 0);
		points_generated++;

		current_global_step++;
		if ((current_global_step - start_global_step) / n_step > total_increments)
		{
			break;
		}
	}

	if ((*segment_data)[*current_size - 1].value < offset || (*segment_data)[*current_size - 1].step != current_global_step - 1)
	{
		if (htr_mode != 1)
		{ // if htr_mode is not OL (without mon)
			if (current_value > htr_max_pwr)
			{
				addPoint(segment_data, current_size, current_capacity, current_global_step, htr_max_pwr, 0);
			}
		}
		else
		{
			addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 0);
		}
		points_generated++;
	}
	return points_generated;
}

// Generates the heater_pattern (sine wave segment)
// Returns the number of points generated for this segment
static int generateSineWaveSegment(uint8_t htr_mode, uint16_t htr_max_pwr, float offset, int dt_amp, int n_period, int nb_period, int start_global_step, HeaterProfilePoint **segment_data, int *current_size, int *current_capacity)
{
	int total_points_segment = n_period * nb_period;
	double angular_step_radians = (2.0 * M_PI) / n_period;
	int current_global_step = start_global_step;
	int points_generated = 0;

	for (int i = 1; i <= total_points_segment; i++)
	{
		double current_angle_radians = i * angular_step_radians;
		double sine_value = offset + dt_amp * sin(current_angle_radians);

		if (sine_value < 0)
		{
			sine_value = 0;
		}
		else if (sine_value > 1023)
		{
			sine_value = 1023;
		}
		// limiter in case of CL or OL_MON
		if (htr_mode != 1)
		{ // if htr_mode is not OL (without mon)
			if (sine_value > htr_max_pwr)
				sine_value = htr_max_pwr;
		}

		fflush(stdout);

		int result = addPoint(segment_data, current_size, current_capacity, current_global_step, sine_value, 1);
		if (result == -1)
		{ // Verificando a falha de alocação
			fflush(stdout);
			// You may want tha function stops here and return points_generated
			// or warn about an error for the function called
			return points_generated; // Return what was generated succesfully
		}
		points_generated++;
		current_global_step++;
	}
	return points_generated;
}

// Generates a downward ramp segment
// Returns the number of points generated for this segment
static int generateDownwardRampSegment(uint8_t htr_mode, uint16_t htr_max_pwr, float offset, float dt_step, int n_step, int start_global_step, HeaterProfilePoint **segment_data, int *current_size, int *current_capacity)
{
	float current_value = offset;
	int current_global_step = start_global_step;
	int decrements_made = 0;
	int points_generated = 0;

	if (dt_step == 0 || offset == 0 || n_step == 0)
	{
		// If no ramp is needed (dt_step=0 or offset=0), just add the starting point at 0.0
		if (htr_mode != 1)
		{ // if htr_mode is not OL (without mon)
			if (offset > htr_max_pwr)
			{
				addPoint(segment_data, current_size, current_capacity, current_global_step, htr_max_pwr, 2);
			}
			else
			{
				addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 2); // Stay at offset or 0 if offset is 0
			}
		}
		points_generated++;
		current_global_step++;
		addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 2);
		points_generated++;
		return 1;
	}

	int total_decrements = (int)(offset / dt_step);
	if (offset > 0 && total_decrements == 0)
	{ // Ensure at least one decrement if offset > 0
		total_decrements = 1;
		decrements_made++;
	}

	if (n_step > 1)
	{
		for (size_t i = 0; i < n_step; i++)
		{
			if (offset > htr_max_pwr)
			{
				addPoint(segment_data, current_size, current_capacity, current_global_step, htr_max_pwr, 2);
			}
			else
			{
				addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 2);
			}
			points_generated++;
			current_global_step++;
		}
	}
	else
	{
		if (offset > htr_max_pwr)
		{
			addPoint(segment_data, current_size, current_capacity, current_global_step, htr_max_pwr, 2);
		}
		else
		{
			addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 2);
		}
		points_generated++;
		current_global_step++;
	}

	while (decrements_made <= total_decrements)
	{
		if ((current_global_step - (start_global_step)) % n_step == 0)
		{
			current_value -= dt_step;
			decrements_made++;
		}
		if (current_value < 0)
		{
			current_value = 0;
		}

		if (current_value > htr_max_pwr)
		{
			addPoint(segment_data, current_size, current_capacity, current_global_step, htr_max_pwr, 2);
		}
		else
		{
			addPoint(segment_data, current_size, current_capacity, current_global_step, current_value, 2);
		}
		points_generated++;

		current_global_step++;
		if (current_value <= 0){
			break;
		}
	}

	// Ensure the last point is exactly 0 if not already included
	if ((*segment_data)[*current_size - 1].value > 0 || (*segment_data)[*current_size - 1].step != current_global_step - 1)
	{
		addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 2);
		points_generated++;
	}
	// }
	return points_generated;
}

static float offset_register_value(uint8_t htr_mode, float offset)
{
	if (htr_mode == 1)// if closed_loop
	{
		return offset * 0.125;
	}
	else
	{
		return offset * 4;
	}
}

static int n_step_register_value(int n_step)
{
	if (n_step <= 32)
	{
		// Keep the value(until 32 the increment is by 1)
		return n_step;
	}
	else if (n_step > 32 && n_step <= 58)
	{
		return (32 + (n_step - 32) * 8);
	}
	else if (n_step > 58 && n_step <= 127)
	{
		return (240 + (n_step - 58)) * 32;
	}
	fprintf(stderr, "Warning: n_step (%d) is out of expected range in n_step_register_value. Returning default/error.\n", n_step);
	return 0;
}

static float dt_step_register_value(uint8_t htr_mode, float dt_step)
{
	int int_dt_step = dt_step / 1;
	switch (int_dt_step)
	{
	case 0:
		if (htr_mode == 1)
		{
			dt_step = 0.125;
		}
		else
		{
			dt_step = 2;
		}
		break;
	case 1:
		if (htr_mode == 1)
		{
			dt_step = 0.25;
		}
		else
		{
			dt_step = 4;
		}
		break;
	case 2:
		if (htr_mode == 1)
		{
			dt_step = 0.5;
		}
		else
		{
			dt_step = 8;
		}
		break;
	case 3:
		if (htr_mode == 1)
		{
			dt_step = 0.75;
		}
		else
		{
			dt_step = 12;
		}
		break;
	case 4:
		if (htr_mode == 1)
		{
			dt_step = 1;
		}
		else
		{
			dt_step = 16;
		}
		break;
	case 5:
		if (htr_mode == 1)
		{
			dt_step = 1.5;
		}
		else
		{
			dt_step = 24;
		}
		break;
	case 6:
		if (htr_mode == 1)
		{
			dt_step = 2;
		}
		else
		{
			dt_step = 32;
		}
		break;
	case 7:
		if (htr_mode == 1)
		{
			dt_step = 3;
		}
		else
		{
			dt_step = 48;
		}
		break;
	default:
		break;
	}

	return dt_step;
}

static int dt_amp_register_value(uint8_t htr_mode, int amp)
{
	switch (amp)
	{
	case 0:
		// keep the value 0
		break;
	case 1:
		if (htr_mode == 1)
		{
			amp = 1;
		}
		else
		{
			amp = 16;
		}
		break;
	case 2:
		if (htr_mode == 1)
		{
			amp = 2;
		}
		else
		{
			amp = 32;
		}
		break;
	case 3:
		if (htr_mode == 1)
		{
			amp = 3;
		}
		else
		{
			amp = 48;
		}
		break;
	case 4:
		if (htr_mode == 1)
		{
			amp = 5;
		}
		else
		{
			amp = 80;
		}
		break;
	case 5:
		if (htr_mode == 1)
		{
			amp = 7;
		}
		else
		{
			amp = 112;
		}
		break;
	case 6:
		if (htr_mode == 1)
		{
			amp = 10;
		}
		else
		{
			amp = 160;
		}
		break;
	case 7:
		if (htr_mode == 1)
		{
			amp = 15;
		}
		else
		{
			amp = 240;
		}
		break;
	default:
		break;
	}
	return amp;
}

static int n_period_register_value(int n_period)
{
	if (n_period <= 15)
	{
		return n_period * 30 + 30;
	}
	else if (n_period > 15)
	{
		return 480 + (n_period - 15) * 120;
	}
	fprintf(stderr, "Warning: n_period (%d) is out of expected range in n_period_register_value. Returning default/error.\n", n_period);
	return 0;
}

static int calc_min_htr_vdd()
{
	// VDD min from documentation
	// VDD_min = 1.55 + 0.0025*MTP.htr_vdd_nvm - 0.02(tolerance set to 20mV)
	//  1.5525     <= MTP.htr_vdd_nvm <= 1.8675
	//  1.53388125 <=     VDD_min     <= 1.53466875

	// Vdd = HTR_VDD * 0.015625
	// Vdd = 98 * 0.015625 = 1.53125  (Under VDD_min independent of MTP.htr_vdd_nvm)
	// Vdd = 99 * 0.015625 = 1.546875 (Over VDD_min independent of MTP.htr_vdd_nvm)
	return 99;
}

// --- Functions exposed on Library (.h) ---

HeaterProfilePoint *generate_combined_profile(
	uint8_t htr_mode,
	uint8_t htr_max_cur,
	float offset,
	int n_step,
	float dt_step,
	int n_period,
	int nb_period,
	int dt_amp,
	int *total_points_count // This will be filled with the total number of points
)
{
	// --- Input validations (Important for libraries) ---
	if (offset < 0)
		return NULL; // Return NULL for error
	if (dt_step < 0 && offset > 0)
		return NULL;
	if (n_step < 0)
		return NULL;
	if (dt_amp < 0)
		return NULL;
	if (n_period < 0)
		return NULL;
	if (nb_period < 0)
		return NULL;

	// Translating the input values to the register related
	offset = offset_register_value(htr_mode, offset);
	dt_step = dt_step_register_value(htr_mode, dt_step);
	n_step = n_step_register_value(n_step);
	dt_amp = dt_amp_register_value(htr_mode, dt_amp);
	n_period = n_period_register_value(n_period);

	uint16_t htr_max_pwr = htr_max_cur / 0.125;

	HeaterProfilePoint *all_signal_points = NULL;
	int current_total_points_count = 0; // Used internally
	int total_points_capacity = 0;

	int current_global_step_counter = 0;

	// 1. Upward Ramp
	generateUpwardRampSegment(htr_mode, htr_max_pwr, offset, dt_step, n_step, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
	if (all_signal_points == NULL)
		return NULL; // Check for realloc failure
	current_global_step_counter = all_signal_points[current_total_points_count - 1].step + 1;

    printf("total_points_capacity after ramp_UP : %i\n", total_points_capacity);

	// 2. Sine Wave
	generateSineWaveSegment(htr_mode, htr_max_pwr, offset, dt_amp, n_period, nb_period, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
	if (all_signal_points == NULL)
		return NULL; // Check for realloc failure
	current_global_step_counter = all_signal_points[current_total_points_count - 1].step + 1;

    printf("total_points_capacity after sinusoid : %i\n", total_points_capacity);

	// 3. Downward Ramp
	generateDownwardRampSegment(htr_mode, htr_max_pwr, offset, dt_step, n_step, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
	if (all_signal_points == NULL)
		return NULL; // Check for realloc failure

    printf("total_points_capacity after ramp_DOWN : %i\n", total_points_capacity);

	// Set the total number of data for the pointer used by the caller
	*total_points_count = current_total_points_count;

	// Return the array alocated dinamically
	return all_signal_points;
}

// Define an initial size for the buffer of error string
// Can be adjusted by the needs, doubling if is enough
#define INITIAL_ERROR_STRING_SIZE 256

char *check_heater_cl_step_report(
	uint32_t htr_temp_device, // the function that is concateneting the temp is delivering it in uint32_t (so we keep ht type when receive to treat it properly inside this function)
	bool compensated)
{
	// Allocate an initial buffer in memory for the string error
	char *error_string = (char *)malloc(INITIAL_ERROR_STRING_SIZE * sizeof(char));
	if (error_string == NULL)
	{
		perror("Failed to allocate memory for the error string");
		return NULL; // Allocating failed
	}
	error_string[0] = 0; // Initialize the string as empty

	size_t current_len = 0;
	size_t buffer_size = INITIAL_ERROR_STRING_SIZE;
	int chars_written;

	if (htr_temp_device & (1 << 23))
	{								   // If the signal bit(24) is equal to 1
		htr_temp_device |= 0xFF000000; //
	}
	float temperature;
	if (compensated)
	{
		temperature = (float)(htr_temp_device / (pow(2, 16)));
	}
	else
	{
		temperature = (float)htr_temp_device;
	}

	printf("Temperature read: %.9f \n", temperature); // TODO temporary debug message

	chars_written = snprintf(error_string + current_len, buffer_size - current_len,
							 "Temperature read: %.9f \n",
							 temperature);

	// If the buffer is not big enough reallocate and try again.
	if (chars_written >= buffer_size - current_len)
	{
		buffer_size = current_len + chars_written + 1; // Novo tamanho necessário
		error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
		if (error_string == NULL)
		{
			perror("Failed to reallocate memory for error string");
			return NULL;
		}
		// Try to write again with the new size
		snprintf(error_string + current_len, buffer_size - current_len,
				 "Temperature read: %.9f \n",
				 temperature);
	}
	current_len = strlen(error_string);

	if (current_len == 0)
	{
		free(error_string);
		return NULL;
	}

	return error_string;
}

char *check_heater_ol_step_report(
	uint8_t htr_mode,
	uint8_t htr_max_cur,
	int htr_power_device,
	float htr_power_model,
	int htr_loop_state,
	int htr_profile_state_device,
	int htr_profile_state_model,
	int htr_vdd,
	int htr_step_model)
{
	// Allocate an initial buffer in memory for the string error
	char *error_string = (char *)malloc(INITIAL_ERROR_STRING_SIZE * sizeof(char));
	if (error_string == NULL)
	{
		perror("Failed to allocate memory for the error string");
		return NULL; // Allocating failed
	}
	error_string[0] = 0; // Initialize the string as empty

	size_t current_len = 0;
	size_t buffer_size = INITIAL_ERROR_STRING_SIZE;
	int chars_written;
	uint16_t htr_max_pwr = htr_max_cur / 0.125;

	int htr_loop_state_expected = 0;
	// Check htr_loop_state value
	if (htr_vdd == calc_min_htr_vdd()) // Voltage limited
	{
		htr_loop_state_expected = 2;
	}
	else if (htr_power_device == htr_max_pwr) // Current limited
	{
		htr_loop_state_expected = 3;
	}
	if (htr_loop_state != htr_loop_state_expected)
	{
		// printf("Error HTR_OL! : htr_profile_state read:  %d, htr_profile_state expected: %d !\n", htr_profile_state_device, htr_profile_state_model);
		// err = 1;

		chars_written = snprintf(error_string + current_len, buffer_size - current_len,
								 "Error HTR_OL! : htr_loop_state read:  %d, htr_loop_state expected: %d !\n",
								 htr_loop_state, htr_loop_state_expected);

		// If the buffer is not big enough reallocate and try again.
		if (chars_written >= buffer_size - current_len)
		{
			buffer_size = current_len + chars_written + 1; // Novo tamanho necessário
			error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
			if (error_string == NULL)
			{
				perror("Failed to reallocate memory for error string");
				return NULL;
			}
			// Try to write again with the new size
			snprintf(error_string + current_len, buffer_size - current_len,
					 "Error HTR_OL! : htr_loop_state read:  %d, htr_loop_state expected: %d !\n",
					 htr_loop_state, htr_loop_state_expected);
		}
		current_len = strlen(error_string);
	}

	// Check htr_profile_state value
	if (htr_profile_state_device != htr_profile_state_model)
	{
		// printf("Error HTR_OL! : htr_profile_state read:  %d, htr_profile_state expected: %d !\n", htr_profile_state_device, htr_profile_state_model);
		// err = 1;

		chars_written = snprintf(error_string + current_len, buffer_size - current_len,
								 "Error HTR_OL! : htr_profile_state read:  %d, htr_profile_state expected: %d !\n",
								 htr_profile_state_device, htr_profile_state_model);

		// If the buffer is not big enough reallocate and try again.
		if (chars_written >= buffer_size - current_len)
		{
			buffer_size = current_len + chars_written + 1; // Novo tamanho necessário
			error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
			if (error_string == NULL)
			{
				perror("Failed to reallocate memory for error string");
				return NULL;
			}
			// Try to write again with the new size
			snprintf(error_string + current_len, buffer_size - current_len,
					 "Error HTR_OL! : htr_profile_state read:  %d, htr_profile_state expected: %d !\n",
					 htr_profile_state_device, htr_profile_state_model);
		}
		current_len = strlen(error_string);
	}

	if (htr_loop_state_expected != 2) // TODO redonsider it after been able to properly calculate the VDD
	{
		if (htr_mode != 1)
		{
			// Check power value
			if (htr_power_device != (int)htr_power_model)
			{
				float htr_power_diff = fabs(htr_power_model - htr_power_device);
				if (htr_power_diff > 1)
				{
					// printf("Error HTR_OL! htr_power read:  %d, htr_power expected: %.2f, htr_power(round) expected: %d !\n", htr_power_device, htr_power_model, (int)htr_power_model);
					// printf("htr_profile_state : %d, htr_profile_step: %d \n", htr_profile_state_device, htr_step_model);
					// printf("Difference between power read and model: %.9f \n", htr_power_diff);
					// err = 1;

					// First message line of error
					chars_written = snprintf(error_string + current_len, buffer_size - current_len,
											 "Error HTR_OL! htr_power read:  %d, htr_power expected: %.2f, htr_power(round) expected: %d !\n",
											 htr_power_device, htr_power_model, (int)htr_power_model);
					if (chars_written >= buffer_size - current_len)
					{
						buffer_size = current_len + chars_written + 1;
						error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
						if (error_string == NULL)
							return NULL;
						snprintf(error_string + current_len, buffer_size - current_len,
								 "Error HTR_OL! htr_power read:  %d, htr_power expected: %.2f, htr_power(round) expected: %d !\n",
								 htr_power_device, htr_power_model, (int)htr_power_model);
					}
					current_len = strlen(error_string);

					// Second message line of error
					chars_written = snprintf(error_string + current_len, buffer_size - current_len,
											 "Error HTR_OL! htr_profile_state : %d, htr_profile_step: %d \n",
											 htr_profile_state_device, htr_step_model);
					if (chars_written >= buffer_size - current_len)
					{
						buffer_size = current_len + chars_written + 1;
						error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
						if (error_string == NULL)
							return NULL;
						snprintf(error_string + current_len, buffer_size - current_len,
								 "Error HTR_OL! htr_profile_state : %d, htr_profile_step: %d \n",
								 htr_profile_state_device, htr_step_model);
					}
					current_len = strlen(error_string);

					// Third message line of error
					chars_written = snprintf(error_string + current_len, buffer_size - current_len,
											 "Error HTR_OL! Difference between power read and model: %.9f \n", htr_power_diff);
					if (chars_written >= buffer_size - current_len)
					{
						buffer_size = current_len + chars_written + 1;
						error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
						if (error_string == NULL)
							return NULL;
						snprintf(error_string + current_len, buffer_size - current_len,
								 "Error HTR_OL! Difference between power read and model: %.9f \n", htr_power_diff);
					}
					current_len = strlen(error_string);
				}
			}
		}
		else
		{
		}
	}

	if (htr_mode == 3)
	{ // 1:CL | 2:OL | 3:OL_MON
		float vdd_real = htr_vdd / (pow(2.0, 6));
		float cur_real = htr_power_device * 0.125; // htr_power_device(dac_code), 0.125(I_lsb = )

		// check vdd
		if (vdd_real < (1.55 + 0.0025 * 1.5525 - 0.02))
		{ // vdd_real < 1.55 + 0.0025*htr_vdd_nvm - tolerance
			float vdd_lim = 1.55 + 0.0025 * 1.5525 - 0.02;
			chars_written = snprintf(error_string + current_len, buffer_size - current_len,
									 "Error HTR_OL! VDD reported less than 1.55 V: %.9f \n", vdd_real);
			if (chars_written >= buffer_size - current_len)
			{
				buffer_size = current_len + chars_written + 1;
				error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
				if (error_string == NULL)
					return NULL;
				snprintf(error_string + current_len, buffer_size - current_len,
						 "Error HTR_OL! VDD reported less than %.9f V: %.9f \n", vdd_lim, vdd_real);
			}
			current_len = strlen(error_string);
		}

		// check current
		if (cur_real > htr_max_cur)
		{
			chars_written = snprintf(error_string + current_len, buffer_size - current_len,
									 "Error HTR_OL! Current reported greater than %d V: %.2f \n", htr_max_cur, cur_real);
			if (chars_written >= buffer_size - current_len)
			{
				buffer_size = current_len + chars_written + 1;
				error_string = (char *)realloc(error_string, buffer_size * sizeof(char));
				if (error_string == NULL)
					return NULL;
				snprintf(error_string + current_len, buffer_size - current_len,
						 "Error HTR_OL! Current reported greater than %d V: %.2f \n", htr_max_cur, cur_real);
			}
			current_len = strlen(error_string);
		}
	}

	if (current_len == 0)
	{
		free(error_string);
		return NULL;
	}

	return error_string;
}

void free_signal_points(HeaterProfilePoint *points)
{
	free(points);
}




// ==========================================Generic_fpga.c==========================================================

/******************************************************************************/
/*!                Macro definition                                           */

/******************************************************************************/
/*!           Static Function Declaration                                     */

/******************************************************************************/
/*!            Functions                                        */

/* This function starts the execution of program. */
int main(void)
{
    int8_t error_code;
    uint8_t read_data_buffer[4], dummy_byte;
    /* An extra dummy byte is read during SPI read */
    uint8_t dummy_byte_len = 0;
    uint8_t reg_addr = 0x0;

    uint8_t htr_temp, htr_n_step, htr_dt_step, htr_n_period,  htr_nb_periods, htr_dt_amp;
    
    //===================================
    // htr open loop direct case
    //===================================
    #ifdef DIRECT_CASE
        // t_step = 266ms; t_dwell = 2.1s; Dtemp = 0x02; Expected length is 4 secs
        // uint8_t n_period[9] = {0, 4, 8, 12, 16, 20, 24, 28, 31};
        // uint8_t nb_periods[9] = {0, 2, 4, 6, 8, 10, 12, 14, 15};
        uint8_t dtemp[9] = {1, 32, 64, 96, 128, 160, 192, 224, 255};
        htr_temp        = 50;
        htr_n_step      = 1;
        htr_dt_step     = 5;
        htr_n_period    = 56; 
        htr_nb_periods  = 1;
        htr_dt_amp      = 2;
        //check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);

    #endif

    uint8_t htr_mode = 2; // 1:CL / 2:OL / 3:OL_MON
    uint8_t htr_mux_cur = 80; // 128mA (=1024 Heater elements)

    int total_heater_pattern_points = 0;
    HeaterProfilePoint* heater_pattern_values = NULL;

    //Heater Model
    heater_pattern_values = generate_combined_profile(htr_mode, htr_mux_cur, htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp, &total_heater_pattern_points);

    if (heater_pattern_values == NULL) {
        printf("\nErro ao gerar o sinal. Verifique os parâmetros de entrada ou houve falha na alocação de memória.\n");
        // return 1;
    }

    for (size_t i = 0; i < total_heater_pattern_points; i++)
    {
        printf("step: %d, value: %.2f, state: %d, i: %d \n", heater_pattern_values[i].step, heater_pattern_values[i].value, heater_pattern_values[i].state, i);
    }

    printf("Size of heater_pattern_values type: %zu\n", sizeof(HeaterProfilePoint));
    printf("Size of list of heater_pattern_values: %zu\n", sizeof(HeaterProfilePoint)*total_heater_pattern_points);
    
    //===================================
    // check htr open loop profile
    //===================================
 
    #ifndef DIRECT_CASE
    //check htr_temp
    uint8_t dtemp[9] = {1, 32, 64, 96, 128, 160, 192, 224, 255};
    for (uint8_t i = 0; i < 9; i++) {
    	htr_temp = dtemp[i];
    	htr_n_step = 1;
    	htr_dt_step = 5;
    	htr_n_period = 0;
    	htr_nb_periods = 1;
    	htr_dt_amp = 2;
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }
    

    //check htr_n_step
    uint8_t n_step[9] = {0, 16, 32, 48, 64, 80, 96, 112, 127};
    for (uint8_t i = 0; i < 9; i++) {
    	htr_temp = 50;
    	htr_n_step = n_step[i];
    	htr_dt_step = 5;
    	htr_n_period = 0;
    	htr_nb_periods = 1;
    	htr_dt_amp = 2;
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }
  
  
  //check htr_dt_step
    uint8_t dt_step[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (uint8_t i = 0; i < 8; i++) {
    	htr_temp = 50;
    	htr_n_step = 1;
    	htr_dt_step = dt_step[i];
    	htr_n_period = 0;
    	htr_nb_periods = 1;
    	htr_dt_amp = 2;
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }
    
    
    //check htr_n_period
    uint8_t n_period[9] = {0, 4, 8, 12, 16, 20, 24, 28, 31};
    for (uint8_t i = 0; i < 9; i++) {
    	htr_temp = 50;
    	htr_n_step = 1;
    	htr_dt_step = 5;
    	htr_n_period = n_period[i];
    	htr_nb_periods = 1;
    	htr_dt_amp = 2;
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }
    

    //check htr_nb_periods
    uint8_t nb_periods[9] = {0, 2, 4, 6, 8, 10, 12, 14, 15};
    for (uint8_t i = 0; i < 9; i++) {
    	htr_temp = 50;
    	htr_n_step = 1;
    	htr_dt_step = 5;
    	htr_n_period = 0;
    	htr_nb_periods = nb_periods[i];
    	htr_dt_amp = 2;
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }
    

    //check htr_dt_amp
    uint8_t dt_amp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (uint8_t i = 0; i < 8; i++) {
    	htr_temp = 50;
    	htr_n_step = 1;
    	htr_dt_step = 5;
    	htr_n_period = 0;
    	htr_nb_periods = 1;
    	htr_dt_amp = dt_amp[i];
    	check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
    }

    #endif
    
    /* Coines interface reset */
    //coines_soft_reset();
    //coines_delay_msec(50);
    //coines_close_comm_intf(comm_intf, NULL);
    return 0;
}
