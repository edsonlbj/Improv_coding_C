#include "stdio.h"
#include <stdbool.h>
//#include "coines.h"

#ifndef _GENERIC_H
#define _GENERIC_H

#define BMI2_SPI_RD_MASK 0x80
#define BMI2_I2C_PRIM_ADDR 0x76

// User Page RegisterMap



// ==========================================Code added by bue1mi==========================================================
// If M_PI is not defined, we can define it manually
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Struct for heater profile data points(step and value)
typedef struct {
    int step;
    float value;
	int state;
} HeaterProfilePoint;

// Main function for the Heater Profile generator
// Returns a pointer to an array of HeaterProfilePoint, and sets total_points_count to the number of generated points
// The "caller" needs to free the memory returned using free_signal_points()
HeaterProfilePoint* generate_combined_profile(
	uint8_t htr_mode,
	uint8_t htr_max_cur,
    float offset,
	int n_step,
    float dt_step,
	int n_period,
	int nb_period,
    int dt_amp,
    int* total_points_count // This will be filled with the total number of points
);

// Function to free the memory alocated by generate_combined_profile()
void free_signal_points(HeaterProfilePoint* points);

//Function to check the heater profile Open_loop
char* check_heater_ol_step_report
(
	uint8_t htr_mode,
	uint8_t htr_max_cur,
	int htr_power_device,
	float htr_power_model,
	int htr_loop_state,
	int htr_profile_state_device,
	int htr_profile_state_model,
	int htr_vdd,
	int htr_step_model
);

//Function to check the heater profile Closed_loop
char *check_heater_cl_step_report
(
	uint32_t htr_temp_device, // the function that is concateneting the temp is delivering it in uint32_t (so we keep ht type when receive to treat it properly inside this function)
	bool compensated
);

#endif
