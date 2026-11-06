void interrupt1_callback(uint32_t param1, uint32_t param2);


#define INVALID_DATA_VALUE 0x7F7F7F
#define DATA_TOLERANCE 4096

typedef enum {
    SENSOR_TEMP1 = 0,
    SENSOR_TEMP2,
    SENSOR_TEMP3,
    SENSOR_THERM,
    SENSOR_BARO,      
    SENSOR_EXTP,     
    SENSOR_WD,        
    SENSOR_DIAG1,     
    SENSOR_DIAG2,     
} sensor_type_t;

typedef struct {
    int32_t  stim_p1b_amp;
    int32_t  stim_p1b_freq;
    int32_t  stim_p1b_ofs;
    int32_t  stim_p12b_amp;
    int32_t  stim_p12b_freq;
    int32_t  stim_p12b_ofs;
    int32_t  stim_t_amp;
    int32_t  stim_t_freq;
    int32_t  stim_t_ofs;
    int32_t  stim_wd_amp;
    int32_t  stim_wd_freq;
    int32_t  stim_wd_ofs;
    int32_t  stim_vdd_amp;
    int32_t  stim_vdd_freq;
    int32_t  stim_vdd_ofs;
} stim_params_t;





uint8_t check_data_vaild(uint32_t data, uint16_t meas_num, uint8_t sens_no);
uint8_t check_data_valid_stim(sensor_type_t sens_no, const stim_params_t *stim, uint32_t data);
uint8_t check_data_invaild(uint32_t data, uint16_t meas_num, uint8_t sens_no);
uint8_t check_data_equal(uint32_t shdw_data, uint32_t fifo_data, uint16_t meas_num, uint8_t sens_no);
void print_stim();
uint8_t check_data_reg_async(uint8_t sens_no, uint32_t shdw_data[9]);
uint8_t check_partial_frame_reading(uint8_t fifo_frame_size, uint8_t *output_frame_data);
uint8_t check_fifo_sens_consistency(uint32_t fifo_data[9][128], uint16_t current_loop, uint16_t *subsmp, uint8_t start_pos[9],int sensor_values[9]);
uint8_t check_FIFO_depth(uint8_t fifo_depth_calc);
uint8_t check_FIFO_frame_size(uint8_t fifo_frame_size_calc);
uint8_t check_interruptStatus();
uint8_t check_FIFO_count(uint16_t level);
void get_otp_comp( uint8_t cfgParam0, char *coeff);
void get_all_otp_comp();