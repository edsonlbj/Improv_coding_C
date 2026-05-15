#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
//#include "coines.h"
#include "generic.h"
#include <math.h>

// #define INFILE 1
#define DIRECT_CASE 1

volatile uint8_t interrupt1_status = 0;

static void interrupt1_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    interrupt1_status = 1;
}

/******************************************************************************/
/*!                Macro definition                                           */

/******************************************************************************/
/*!           Static Function Declaration                                     */

/******************************************************************************/
/*!            Functions                                        */
static void check_heater_ol(uint8_t htr_temp, uint8_t htr_n_step, uint8_t htr_dt_step, uint8_t htr_n_period, 
uint8_t htr_nb_periods, uint8_t htr_dt_amp){

    // Heater Enable
    uint8_t subsmp = 1;
    uint8_t start_pos = 0;
    uint8_t suspend_run = 0;
    uint8_t htr_mode = 2; // 1:CL / 2:OL / 3:OL_MON
    uint8_t htr_mux_cur = 80; // 128mA (=1024 Heater elements)
	//cfg_heaterSubsmp(subsmp, start_pos, suspend_run);
	//cfg_heaterMode(htr_mode);
	//cfg_heaterMaxCur(htr_mux_cur);

    uint8_t data_reg[3];
    uint16_t htr_power = 0;
    uint8_t htr_loop_state;
    uint8_t htr_profile_state;
    uint8_t htr_vdd;

    uint8_t htr_err = 0;
    uint8_t odr_cfg = 41; //30Hz

    int total_heater_pattern_points = 0;
    HeaterProfilePoint* heater_pattern_values = NULL;

    // Heater Config
	cfg_heaterProfiler(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);

    //Heater Model
    heater_pattern_values = generate_combined_profile(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp, &total_heater_pattern_points);

    if (heater_pattern_values == NULL) {
        printf("\nErro ao gerar o sinal. Verifique os parâmetros de entrada ou houve falha na alocação de memória.\n");
        // return 1;
    }

    // for (size_t i = 0; i < total_heater_pattern_points; i++)
    // {
    //     printf("i = %d, HTR_Step = %d, HTR_Value = %.2f, HTR_state = %d\n", i, heater_pattern_values[i].step, heater_pattern_values[i].value, heater_pattern_values[i].state);
    // }

    for (size_t i = 0; i < total_heater_pattern_points; i++)
    {
        printf("%d  %.2f\n", heater_pattern_values[i].step, heater_pattern_values[i].value);
    }

    free_signal_points(heater_pattern_values);

    // ========================================
    // Start triggering free-run mode
    // ========================================

    #ifdef INFILE

        uint32_t time_us = 0 ;

        // oprn flash file
	    char case_name[150];
        sprintf(case_name, "HTR_OL_dtemp%d_nstep%d_dtstep%d_npeiod%d_nbperiod%d_dtamp%d", htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
        printf("Test case: %s\n ", case_name);
	    char file_name[50];
	    sprintf(file_name, "HTR_%d_%d_%d_%d_%d_%d.txt", htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);
	    remove(file_name);
	    FILE *fp = fopen(file_name,"w");
	    // turn on green led once log file start
	    coines_set_led(COINES_LED_GREEN, COINES_LED_STATE_ON);

        //trig free-run mode
        odrConfig(1, odr_cfg);

        // read heater feature
        while(1) {
            if (interrupt1_status) {
                time_us = coines_get_realtime_usec() ;
	    		interrupt1_status = 0;
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);

                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];
                fprintf(fp,"%ld 0x%04x %d %d %d\n",time_us, htr_power, htr_loop_state, htr_profile_state, htr_vdd);
                if ((htr_power == 0 && htr_profile_state == 2) || htr_loop_state == 7) 
                {
                    break;
                }  
            }
        }

        // check htr error
        (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_ERROR_ADDR | BMI2_SPI_RD_MASK), &htr_err, 1);

        if (htr_err == 0)
            fprintf(fp, "Heater finished without errors");
        else
            fprintf(fp, "Heater finished with errors: %02x",htr_err);

        odrConfig(0, odr_cfg);
        (void)coines_set_shuttleboard_vdd_vddio_config(0, 0);
        coines_delay_msec(100);
        fclose(fp);

    #else

        /*
        // trig free-run mode
        uint16_t loop = 0;
        printf("Enable Heater and Start Measurement...\n");
        printf("Trigger odr with 30Hz\n");
        odrConfig(1, 41);

        // read heater feature
        while(1) {
            if (interrupt1_status) {
                loop++;
	    		interrupt1_status = 0;

                // write user regs when heater is started
                if (loop == 1) {
                    printf("Write DATA_CH_SEL\n");
                    cfg_data_ch_sel(TEMP1);
                    cfg_data_ch_sel(HEATER);
                }
                else if (loop == 2) {
                    cfg_interrupt(EMU_INT_CONFIG_INT_POL_LOW | EMU_INT_CONFIG_INT_MODE_LATCHED);
                    cfg_interrupt(int_config);
                    printf("Write INT_CONFIG\n");
                }
                else if (loop == 3) {
                    enb_interrupt(EMU_INT_SOURCE_DRDY_DATA_REG_EN | EMU_INT_SOURCE_HTR_ERR_EN);
                    printf("Write INT_SOURCE\n");
                }
                else if (loop == 4) {
                    cfg_fifo(0, EMU_FIFO_CONFIG_FIFO_MODE_SOF);
                    printf("Write FIFO_CONFIG\n");
                }
                else if (loop == 5) {
                    cfg_fifoFrameMsb(EMU_FIFO_FRAME_CFG_MSB_HEATER);
                    cfg_fifoFrameLsb(EMU_FIFO_FRAME_CFG_LSB_BARO);
                    printf("Write FIFO_FRAME_CFG_MSB/LSB\n");
                }
                else if(loop == 6) {
                    uint8_t wd_th = 0xff;
                    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_WD_LTH_LSB_ADDR, &wd_th, 1);
                    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_WD_LTH_MSB_ADDR, &wd_th, 1);
                    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_WD_UTH_LSB_ADDR, &wd_th, 1);
                    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_WD_UTH_MSB_ADDR, &wd_th, 1);
                    printf("Write WD_*_*\n");
                }

                // read data
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];
                printf("htr_power = %d, htr_loop_state = %d, htr_profile_state = %d, htr_vdd = %d \n", htr_power, htr_loop_state, htr_profile_state, htr_vdd);
                // printf("HTR_Value = %.2f, HTR_Step = %d, HTR_state_model = %d, \n", heater_pattern_values[loop].value, heater_pattern_values[loop].step, heater_pattern_values[loop].state);
                check_heater_ol_step_report(htr_power, heater_pattern_values[loop].value, htr_loop_state, htr_profile_state, heater_pattern_values[loop].state, htr_vdd, heater_pattern_values[loop].step);
                // if (htr_power == 0) {
                if (htr_power == 0 && htr_profile_state == 2) {
                    break;
                }
                // if (loop == total_heater_pattern_points-1) {
                //     break;
                // }
            }
        }

        free_signal_points(heater_pattern_values);

	    //Check that Heater finished w/o errorss
        (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_ERROR_ADDR | BMI2_SPI_RD_MASK), &htr_err, 1);
        if (htr_err == 0)
            printf("Heater finished without errors\n");
        else
            printf("Heater finished with errors: %02x\n",htr_err);
        
        odrConfig(0, odr_cfg);
        coines_delay_msec(500);

        printf("\nRe-Enable Heater and Start Measurement...\n");
        cfg_heaterMode(htr_mode);
        printf("Trigger odr with 30Hz\n");
        odrConfig(1, 41);
        loop = 0;
        while(1) {
            if (interrupt1_status) {
	    		interrupt1_status = 0;
                loop++;
                // read data
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];
                printf("htr_power = %d, htr_loop_state = %d, htr_profile_state = %d, htr_vdd = %d \n", htr_power, htr_loop_state, htr_profile_state, htr_vdd);
                check_heater_ol_step_report(htr_power, heater_pattern_values[loop].value, htr_loop_state, htr_profile_state, heater_pattern_values[loop].state, htr_vdd, heater_pattern_values[loop].step);
                if (htr_power != 0 && htr_profile_state == 1) {
                    break;
                }
            }
        }
        printf("Disenable Heater during heater running via reconfiguration\n");
        printf("Trigger odr with 30Hz\n");
        odrConfig(1, 41);
        while(1) {
            if (interrupt1_status) {
	    		interrupt1_status = 0;
                // read data
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];
                printf("htr_power = %d, htr_loop_state = %d, htr_profile_state = %d, htr_vdd = %d \n", htr_power, htr_loop_state, htr_profile_state, htr_vdd);
                check_heater_ol_step_report(htr_power, heater_pattern_values[loop].value, htr_loop_state, htr_profile_state, heater_pattern_values[loop].state, htr_vdd, heater_pattern_values[loop].step);
                if (htr_loop_state == 7) {
                    break;
                }
            }
        }
        //Check that Heater finished w/o errorss
        (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_ERROR_ADDR | BMI2_SPI_RD_MASK), &htr_err, 1);
        if (htr_err == 0)
            printf("Heater finished without errors\n");
        else
            printf("Heater finished with errors: %02x\n",htr_err);

        odrConfig(0, odr_cfg);
        coines_delay_msec(300);
        */
    #endif
	
}



/* This function starts the execution of program. */
int main(void)
{

    
    //int8_t error_code;
    //uint8_t read_data_buffer[4], dummy_byte;
    /* An extra dummy byte is read during SPI read */
    //uint8_t dummy_byte_len = 0;
    //uint8_t reg_addr = 0x0;
    
    //enum coines_comm_intf comm_intf = COINES_COMM_INTF_USB;
    
    //error_code = coines_open_comm_intf(comm_intf, NULL);
    //if (error_code == COINES_SUCCESS)
    //{printf("\nSuccessfully connected to board!\n");
   // }
    //else
   // {
    //printf("\nUnable to connect with board!\n");
    //exit(error_code);
   // }
    
    /* Power up the board */
    //coines_set_shuttleboard_vdd_vddio_config(3300, 3300);
    //coines_delay_msec(200);
    /* SPI config */
    //(void)coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_10_MHZ, COINES_SPI_MODE0);
    //printf("SPI Access ...\r\n");
    /* Regsiter callback function for INT1 with falling edge detection*/
	//coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6,interrupt1_callback,COINES_PIN_INTERRUPT_FALLING_EDGE);
    /* Pin config */
    //coines_set_pin_config(COINES_SHUTTLE_PIN_21, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
    /* Mask read register address for SPI */
    //reg_addr = (reg_addr | BMI2_SPI_RD_MASK);
    /* Dummy read for SPI init*/
    //(void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, &dummy_byte, 1);
    //coines_delay_usec(450);
    
    
    // Read CHIP_ID & MANU_ID
    //reg_addr = 0x00;
    //reg_addr = (reg_addr | BMI2_SPI_RD_MASK);
    //(void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, read_data_buffer, 2 + dummy_byte_len);
    //printf("MANU_ID = 0x%02x\n", read_data_buffer[0]);	
    //printf("CHIP_ID = 0x%02x\n", read_data_buffer[1]);	
    //coines_delay_msec(500);

    /* Secondary SPI configuration */  
    uint32_t stim_p1b_amp   = 2560000;  // 10KPa  
    uint32_t stim_p1b_freq  = 1280;     // 10Hz
    uint32_t stim_p1b_ofs   = 25600000; // 100KPa      // 25600000; // 100KPa   // 20480000; // 80KPa  
    uint32_t stim_p12b_amp  = 76800000; // 300KPa      // 25600000; // 100KPa    // 76800000; // 300KPa
    uint32_t stim_p12b_freq = 1280;     // 10Hz        // 25600;    // 100Hz     // 1280;     // 10Hz
    uint32_t stim_p12b_ofs  = 153600000; // 600KPa 
    uint32_t stim_t_amp     = 0;        // 671088640;  // 40degC
    uint32_t stim_t_freq    = 0;        // 1280;       // 10KHz
    uint32_t stim_t_ofs     = 0;        // 0degC       // 4009754624; // -17degC  // 419430400; // 25degC
    uint32_t stim_wd_amp    = 1677777;  // 0.1
    uint32_t stim_wd_freq   = 5120;     // 20*2^8
    uint32_t stim_wd_ofs    = 0;
    uint32_t stim_vdd_amp   = 0;
    uint32_t stim_vdd_freq  = 0;
    uint32_t stim_vdd_ofs   = 36909875; // 33554432; // 2.0      // 36909875; // 2.2   //41943040; // 2.5    //42788534; // 2.61 


    /*stimReg_init(stim_p1b_amp,  stim_p1b_freq, stim_p1b_ofs,
				 stim_p12b_amp, stim_p12b_freq,stim_p12b_ofs,
				 stim_t_amp,    stim_t_freq,   stim_t_ofs,
				 stim_wd_amp,   stim_wd_freq,  stim_wd_ofs,
				 stim_vdd_amp,  stim_vdd_freq, stim_vdd_ofs);
    */
    /* *************************************
    Start Heater Open Loop test
    ************************************** */
    
    uint8_t htr_temp, htr_n_step, htr_dt_step, htr_n_period,  htr_nb_periods, htr_dt_amp;
    
    //===================================
    // htr open loop direct case
    //===================================
    #ifdef DIRECT_CASE
        // t_step = 266ms; t_dwell = 2.1s; Dtemp = 0x02; Expected length is 4 secs
        uint8_t dtemp[9] = {1, 32, 64, 96, 128, 160, 192, 224, 255};
        htr_temp        = dtemp[3];
        htr_n_step      = 1;
        htr_dt_step     = 5;
        htr_n_period    = 0;
        htr_nb_periods  = 1;
        htr_dt_amp      = 2;
        check_heater_ol(htr_temp, htr_n_step, htr_dt_step, htr_n_period, htr_nb_periods, htr_dt_amp);

        
    #endif
    
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
