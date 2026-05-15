
#include "generic.h"
//#include "coines.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include "stdio.h"
#endif


/***************************************************************************/

/*!              Static Variable
 ****************************************************************************/

/******************************************************************************/

/*!         Local Function Prototypes
 ******************************************************************************/


// ==========================================Code added by bue1mi==========================================================

// --- Aux Intern Functions (not exposed in .h) 

// Helper function to dynamically add a point to an array
// Returns the new size of the array
static int addPoint(HeaterProfilePoint** arr, int* current_size, int* current_capacity, int step, float value, int state) 
{
    if (*current_size >= *current_capacity) {
        *current_capacity *= 2; // Double the capacity
        if (*current_capacity == 0) *current_capacity = 10; // Initial capacity if it was 0
        *arr = (HeaterProfilePoint*)realloc(*arr, (*current_capacity) * sizeof(HeaterProfilePoint));
        if (*arr == NULL) {
            perror("Failed to reallocate memory for signal points");
            exit(EXIT_FAILURE); //In a real librery, would return an error.
        }
    }
    (*arr)[*current_size].step = step;
    (*arr)[*current_size].value = value;
	(*arr)[*current_size].state = state;
    (*current_size)++;
    return *current_size;
}

// Generates an upward ramp segment
// Returns the number of points generated for this segment
static int generateUpwardRampSegment(float offset, float dt_step, int n_step, int start_global_step, HeaterProfilePoint** segment_data, int* current_size, int* current_capacity)
{

	printf("offset: %.2f, dt_step: %.2f, n_step: %d, start_global_step: %d \n", offset, dt_step, n_step, start_global_step);
    float current_value = 0.0;
    int current_global_step = start_global_step;
    int increments_made = 0;
    int points_generated = 0;

    if (dt_step == 0 || offset == 0) {
        // If no ramp is needed (dt_step=0 or offset=0), just add the starting point at 0.0
        addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 0);
        return 1;
    }

    int total_increments = (int)(offset / dt_step);
    if (offset > 0 && total_increments == 0){ // Ensure at least one increment if offset > 0 and dt_step is large
		total_increments = 1;
		increments_made++;
	} 

    addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 0);
    points_generated++;
    current_global_step++;

    while (increments_made < total_increments) {
        if ((current_global_step - start_global_step) % n_step == 0) {
            current_value += dt_step;
            increments_made++;
        }
        if (current_value > offset) {
            current_value = offset;
        }
        
        addPoint(segment_data, current_size, current_capacity, current_global_step, current_value, 0);
        printf("!!!!!!!!!HEATER_step: %d, HEATER_value: %.2f, offset: %f \n", (*segment_data)[*current_size - 1].step, (*segment_data)[*current_size - 1].value, offset);
		printf("!!!!!!!!!step: %d \n",  (*segment_data)[*current_size - 1].step);
		printf("!!!!!!!!!Current_global_step: %d \n", current_global_step);
		points_generated++;

		// //debugging messages
		// printf("RAMP_UP: Current Value = %.2f, points generated = %d, Global Step = %d \n", current_value, points_generated, current_global_step);
		// //debugging
        if (current_value >= offset) break;
        current_global_step++;
		// //debugging messages
		// printf("RAMP_UP: Current Value = %.2f, points generated = %d, Global Step = %d \n", current_value, points_generated, current_global_step);
		// //debugging
		// printf("!!!!!!!!!HEATER_step: %d, HEATER_value offset: %.2f, offset: %f \n", (*segment_data)[*current_size - 1].step, (*segment_data)[*current_size - 1].value, offset);
		// printf("!!!!!!!!!step: %d \n",  (*segment_data)[*current_size - 1].step);
		// printf("!!!!!!!!!Current_global_step: %d \n", current_global_step);
    }
    // Ensure the last point is exactly the offset if not already included
    if ((*segment_data)[*current_size - 1].value < offset || (*segment_data)[*current_size - 1].step != current_global_step-1) {
        
		printf("!!!!!!!!!(*segment_data)[*current_size - 1].value: %.2f < offset: %.2f \n", (*segment_data)[*current_size - 1].value, offset);
		printf("!!!!!!!!!(*segment_data)[*current_size - 1].step: %d != current_global_step-1: %d \n", (*segment_data)[*current_size - 1].step, current_global_step-1);
		
		addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 0);
        points_generated++;
		// //debugging messages
		// printf("RAMP_UP(ensure): Current Value = %.2f, points generated = %d, Global Step = %d \n", offset, points_generated, current_global_step);
		// //debugging
    }
    return points_generated;
}

// Generates the heater_pattern (sine wave segment)
// Returns the number of points generated for this segment
static int generateSineWaveSegment(float offset, int dt_amp, int n_period, int nb_period, int start_global_step, HeaterProfilePoint** segment_data, int* current_size, int* current_capacity)
{
    int total_points_segment = n_period * nb_period;
    float angular_step_radians = (2.0 * M_PI) / n_period;
    int current_global_step = start_global_step;
    int points_generated = 0;

    for (int i = 1; i < total_points_segment; i++) {
        float current_angle_radians = i * angular_step_radians;
        float sine_value = offset + dt_amp * sin(current_angle_radians);
		// if (mode == open_loop) {
			if (sine_value < 0) { 
				sine_value = 0;
			}
		// }
		
        addPoint(segment_data, current_size, current_capacity, current_global_step, sine_value, 1);
        points_generated++;
        current_global_step++;
		// //debugging messages
		// printf("SineWave: Sine Value = %.2f, points generated = %d, Global Step = %d \n", sine_value, points_generated, current_global_step);
		// //debugging
    }
    return points_generated;
}

// Generates a downward ramp segment
// Returns the number of points generated for this segment
static int generateDownwardRampSegment(float offset, float dt_step, int n_step, int start_global_step, HeaterProfilePoint** segment_data, int* current_size, int* current_capacity)
{
    float current_value = offset;
    int current_global_step = start_global_step;
    int decrements_made = 0;
    int points_generated = 0;

    if (dt_step == 0 || offset == 0) {
        // If no ramp is needed (dt_step=0 or offset=0), just add the starting point at 0.0
        addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 2); // Stay at offset or 0 if offset is 0
        return 1;
    }

    int total_decrements = (int)(offset / dt_step);
    if (offset > 0 && total_decrements == 0){ // Ensure at least one decrement if offset > 0
		total_decrements = 1;
		decrements_made++;
	}

    addPoint(segment_data, current_size, current_capacity, current_global_step, offset, 2);
    points_generated++;
    current_global_step++;

    while (decrements_made < total_decrements) {
        if ((current_global_step - (start_global_step + 1)) % n_step == 0) {
            current_value -= dt_step;
            decrements_made++;
        }
        if (current_value < 0) {
            current_value = 0;
        }

        addPoint(segment_data, current_size, current_capacity, current_global_step, current_value, 2);
        points_generated++;

		// //debugging messages
		// printf("RampDown: Current Value = %.2f, points generated = %d, Global Step = %d \n", current_value, points_generated, current_global_step);
		// //debugging

        if (current_value <= 0) break;
        current_global_step++;
		// //debugging messages
		// printf("RampDown: Current Value = %.2f, points generated = %d, Global Step = %d \n", current_value, points_generated, current_global_step);
		// //debugging
    }
    // Ensure the last point is exactly 0 if not already included
    if ((*segment_data)[*current_size - 1].value > 0 || (*segment_data)[*current_size - 1].step != current_global_step-1) {
        addPoint(segment_data, current_size, current_capacity, current_global_step, 0.0f, 2);
        points_generated++;
		// //debugging messages
		// printf("RampDown: Current Value = %.2f, points generated = %d, Global Step = %d \n", 0.0f, points_generated, current_global_step);
		// //debugging
    }
    return points_generated;
}

static float offset_register_value(float offset)
{
	//if open_loop
	return offset*4;
	// else 
	// return offset*0.125;
}

static int n_step_register_value(int n_step)
{
	if (n_step <= 32) {
		//Keep the value(until 32 the increment is by 1)
		return n_step;
	} else if (n_step>32 && n_step<=58) {
		return (32+(n_step-32)*8);
	} else if (n_step>58 && n_step<=127) {
		return (240+(n_step-58))*32;
	}
	fprintf(stderr, "Warning: n_step (%d) is out of expected range in n_step_register_value. Returning default/error.\n", n_step);
    return 0;
}

static float dt_step_register_value(float dt_step)
{
	int int_dt_step = dt_step/1;
	switch (int_dt_step)
	{
		case 0:
			dt_step = 2;
			//if closed_loop dt_step = 0.125;
			break;
		case 1:
			dt_step = 4;
			//if closed_loop dt_step = 0.25;
			break;
		case 2:
			dt_step = 8;
			//if closed_loop dt_step = 0.5;
			break;
		case 3:
			dt_step = 12;
			//if closed_loop dt_step = 0.75;
			break;
		case 4:
			dt_step = 16;
			//if closed_loop dt_step = 1;
			break;
		case 5:
			dt_step = 24;
			//if closed_loop dt_step = 1.5;
			break;
		case 6:
			dt_step = 32;
			//if closed_loop dt_step = 2;
			break;
		case 7:
			dt_step = 48;
			//if closed_loop dt_step = 3;
			break;
		default:
			break; 
	}

	return dt_step;
}

static int dt_amp_register_value(int amp)
{
	switch (amp)
	{
	case 0:
		//keep the value 0
		break;
	case 1:
		amp = 16;
		// if closed_loop amp = 1;
		break;
	case 2:
		amp = 32;
		// if closed_loop amp = 2;
		break;
	case 3:
		amp = 48;
		// if closed_loop amp = 3;
		break;
	case 4:
		amp = 80;
		// if closed_loop amp = 5;
		break;
	case 5:
		amp = 112;
		// if closed_loop amp = 7;
		break;
	case 6:
		amp = 160;
		// if closed_loop amp = 10;
		break;
	case 7:
		amp = 240;
		// if closed_loop amp = 15;
		break;
	default:
		break;
	}
	return amp;
}

static int n_period_register_value(int n_period)
{
	if (n_period <= 15) {
		return n_period*30+30;
	} else if (n_period>15) {
		return 480+(n_period-15)*120;
	}
	fprintf(stderr, "Warning: n_period (%d) is out of expected range in n_period_register_value. Returning default/error.\n", n_period);
    return 0;
}

// --- Functions exposed on Library (.h) ---

HeaterProfilePoint* generate_combined_profile(
    float offset,
	int n_step,
    float dt_step,
	int n_period,
	int nb_period,
    int dt_amp,
    int* total_points_count // This will be filled with the total number of points
) {
    // --- Input validations (Important for libraries) ---
    if (offset < 0) return NULL; // Return NULL for error
    if (dt_step <= 0 && offset > 0) return NULL;
    if (n_step < 0) return NULL;
    if (dt_amp < 0) return NULL;
    if (n_period < 0) return NULL;
    if (nb_period < 0) return NULL;

	//Translating the input values to the register related
	offset = offset_register_value(offset);
	dt_step = dt_step_register_value(dt_step);
	n_step = n_step_register_value(n_step);
	dt_amp = dt_amp_register_value(dt_amp);
	n_period = n_period_register_value(n_period);

    HeaterProfilePoint* all_signal_points = NULL;
    int current_total_points_count = 0; // Used internally
    int total_points_capacity = 0;

    int current_global_step_counter = 0;

    // 1. Upward Ramp
    generateUpwardRampSegment(offset, dt_step, n_step, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
    if (all_signal_points == NULL) return NULL; // Check for realloc failure
    current_global_step_counter = all_signal_points[current_total_points_count-1].step + 1;

    // 2. Sine Wave
    generateSineWaveSegment(offset, dt_amp, n_period, nb_period, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
    if (all_signal_points == NULL) return NULL; // Check for realloc failure
    current_global_step_counter = all_signal_points[current_total_points_count-1].step + 1;

    // 3. Downward Ramp
    generateDownwardRampSegment(offset, dt_step, n_step, current_global_step_counter, &all_signal_points, &current_total_points_count, &total_points_capacity);
    if (all_signal_points == NULL) return NULL; // Check for realloc failure

    // Set the total number of data for the pointer used by the caller
    *total_points_count = current_total_points_count;

	// Return the array alocated dinamically
    return all_signal_points;
}
/*
uint8_t check_heater_ol_step_report
(
	int htr_power_device,
	float htr_power_model,
	int htr_loop_state,
	int htr_profile_state_device,
	int htr_profile_state_model,
	int htr_vdd,
	int htr_step_model
)
{
    uint8_t err = 0;
	//Check htr_profile_state value
    if (htr_profile_state_device != htr_profile_state_model)
    {
        printf("Error HTR_OL! : htr_profile_state read:  %d, htr_profile_state expected: %d !\n", htr_profile_state_device, htr_profile_state_model);
        err = 1;
    }
	//Check power value
    if (htr_power_device != (int)htr_power_model)
    {
		float htr_power_diff = fabs(htr_power_model - htr_power_device);
		if (htr_power_diff > 1) {
			printf("Error HTR_OL! htr_power read:  %d, htr_power expected: %.2f, htr_power(round) expected: %d !\n", htr_power_device, htr_power_model, (int)htr_power_model);
			printf("htr_profile_state : %d, htr_profile_step: %d \n", htr_profile_state_device, htr_step_model);
			printf("Difference between power read and model: %.9f \n", htr_power_diff);
			err = 1;
		}
    }
    return err;
}
*/
void free_signal_points(HeaterProfilePoint* points) {
    free(points);
}
