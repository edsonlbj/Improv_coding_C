#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "coines.h"
#include "generic_fpga.h"
#include "generic.h"
#include "horizon_addrmap.h"

/******************************************************************************/
/*!                Macro definition                                           */
// Default setting
#define HTR_MODE 1 // 1:CL / 2:OL / 3:OL_MON
#define HTR_MAX_CUR 80

#define HTR_DT_TEMP 32 // delt-T = 4degC
#define HTR_N_STEP 4
#define HTR_DT_STEP 2  // 0.5degC
#define HTR_N_PERIOD 1 // 2s 1s/30Hz
#define HTR_NB_PERIOD 3
#define HTR_DT_AMP 1 // 1degC

#define TEMP1_SUBSMP 1      // 1
#define TEMP1_START_POS 0   // 0
#define TEMP1_SUSPEND_RUN 0 // 0
#define TEMP1_OVS_OFS 0     // 0
#define TEMP1_OVS_MULT 1    // 1
#define TEMP1_CIC_DEC 0     // 0

#define BARO_SUBSMP 1      // 1
#define BARO_START_POS 0   // 0
#define BARO_SUSPEND_RUN 0 // 0
#define BARO_OVS_OFS 0     // 0
#define BARO_OVS_MULT 1    // 1
#define BARO_CIC_DEC 0     // 0

#define WD_SUBSMP 1      // 1
#define WD_START_POS 0   // 0
#define WD_SUSPEND_RUN 0 // 0
#define WD_OVS_OFS 0     // 0
#define WD_OVS_MULT 1    // 1
#define WD_CIC_DEC 0     // 0

#define HTR_SUBSMP 1
#define HTR_START_POS 0
#define HTR_SUSPEND_RUN 0

#define DIAG1_SUBSMP 1
#define DIAG1_CFG 57
#define DIAG1_START_POS 0
#define DIAG1_SUSPEND_RUN 0
#define DIAG1_OVS_OFS 0
#define DIAG1_OVS_MULT 3
#define DIAG1_CIC_DEC 3

#define ODR_CFG 41 // 30Hz

#define T_ofs 0          // 0; // 0degC       // 4009754624; // -17degC  // 419430400; // 25degC
#define VDD_ofs 36909875 // 36909875; // 2.2v        // 33554432; // 2.0      // 36909875; // 2.2   //41943040; // 2.5    //42788534; // 2.61

// #define USE_CASE_CL 1
// #define USE_CASE_OL 1
#ifdef USE_CASE_CL
#undef TEMP1_SUBSMP
#define TEMP1_SUBSMP 2
#undef TEMP1_OVS_MULT
#define TEMP1_OVS_MULT 0

#undef WD_SUBSMP
#define WD_SUBSMP 4
#undef WD_OVS_MULT
#define WD_OVS_MULT 0

#undef BARO_OVS_MULT
#define BARO_OVS_MULT 0

#undef HTR_SUBSMP
#define HTR_SUBSMP 4

#undef HTR_DT_TEMP
#define HTR_DT_TEMP 8
#undef HTR_DT_STEP
#define HTR_DT_STEP 0
#undef HTR_NB_PERIOD
#define HTR_NB_PERIOD 2
#undef HTR_DT_AMP
#define HTR_DT_AMP 0

#undef ODR_CFG
#define ODR_CFG 67 // 4*30hz = 120Hz

#elif defined(USE_CASE_OL)
#undef HTR_MODE
#define HTR_MODE 2

#undef TEMP1_SUBSMP
#define TEMP1_SUBSMP 2
#undef TEMP1_OVS_MULT
#define TEMP1_OVS_MULT 0

#undef WD_SUBSMP
#define WD_SUBSMP 4
#undef WD_OVS_MULT
#define WD_OVS_MULT 0

#undef BARO_OVS_MULT
#define BARO_OVS_MULT 0

#undef HTR_SUBSMP
#define HTR_SUBSMP 4

#undef HTR_DT_TEMP
#define HTR_DT_TEMP 2
#undef HTR_N_STEP
#define HTR_N_STEP 8
#undef HTR_DT_STEP
#define HTR_DT_STEP 0
#undef HTR_NB_PERIOD
#define HTR_NB_PERIOD 1
#undef HTR_DT_AMP
#define HTR_DT_AMP 0

#undef ODR_CFG
#define ODR_CFG 67 // 4*30hz = 120Hz

#endif

// #define INFILE 1
// #define FILE_NAME "SHDW_HTR_CL_30Hz.txt"
// #define READ_FIFO
// #define RECONFIG 1

/******************************************************************************/
/*!           Static Function Declaration                                     */
volatile uint8_t interrupt1_status = 0;

void interrupt1_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    interrupt1_status = 1;
}

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

    enum coines_comm_intf comm_intf = COINES_COMM_INTF_USB;

    error_code = coines_open_comm_intf(comm_intf, NULL);
    if (error_code == COINES_SUCCESS)
    {
        printf("\nSuccessfully connected to board!\n");
    }
    else
    {
        printf("\nUnable to connect with board!\n");
        exit(error_code);
    }

    /* Power up the board */
    coines_set_shuttleboard_vdd_vddio_config(2200, 1800);
    coines_delay_msec(200);
    /* SPI config */
    (void)coines_config_spi_bus(COINES_SPI_BUS_0, COINES_SPI_SPEED_10_MHZ, COINES_SPI_MODE0);
    printf("SPI Access ...\r\n");
    /* Regsiter callback function for INT1 with falling edge detection*/
    coines_attach_interrupt(COINES_MINI_SHUTTLE_PIN_1_6, interrupt1_callback, COINES_PIN_INTERRUPT_FALLING_EDGE);

    /* Pin config */
    coines_set_pin_config(COINES_SHUTTLE_PIN_21, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
    /* Mask read register address for SPI */
    reg_addr = (reg_addr | BMI2_SPI_RD_MASK);
    /* Dummy read for SPI init*/
    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, &dummy_byte, 1);
    coines_delay_usec(450);

    /* Read CHIP_ID & MANU_ID */
    reg_addr = 0x00;
    reg_addr = (reg_addr | BMI2_SPI_RD_MASK);
    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, read_data_buffer, 2 + dummy_byte_len);
    printf("MANU_ID = 0x%02x\n", read_data_buffer[0]);
    printf("CHIP_ID = 0x%02x\n", read_data_buffer[1]);
    coines_delay_msec(500);

    /* Interrupt Configuration */
    uint8_t int_config = EMU_INT_CONFIG_INT_POL_LOW | EMU_INT_CONFIG_INT_MODE_PULSED;
    uint8_t int_source = EMU_INT_SOURCE_FIFO_THS_EN | EMU_INT_SOURCE_DRDY_DATA_REG_EN | EMU_INT_SOURCE_HTR_ERR_EN;
    cfg_interrupt(int_config);
    enb_interrupt(int_source);

    /* FIFO Configuration */
    uint8_t fifo_thres = 0x2;
    cfg_fifo(fifo_thres, EMU_FIFO_CONFIG_FIFO_MODE_STR);

    uint8_t fifo_frame_cfg_lsb = EMU_FIFO_FRAME_CFG_LSB_TEMP1 | EMU_FIFO_FRAME_CFG_LSB_BARO | EMU_FIFO_FRAME_CFG_LSB_WD;
    uint8_t fifo_frame_cfg_msb = EMU_FIFO_FRAME_CFG_MSB_HEATER;
    cfg_fifoFrameLsb(fifo_frame_cfg_lsb);
    cfg_fifoFrameMsb(fifo_frame_cfg_msb);

    /* COMP Configuration */
    uint8_t comp = 0x71;
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_COMP_ADDR, &comp, 1);

    int total_heater_pattern_points = 0;
    HeaterProfilePoint *heater_pattern_values = NULL;

    /* Heater Configuration */
    cfg_heaterSubsmp(HTR_SUBSMP, 0, 0);
    cfg_heaterMode(HTR_MODE);
    cfg_heaterMaxCur(HTR_MAX_CUR);
    cfg_heaterProfiler(HTR_DT_TEMP, HTR_N_STEP, HTR_DT_STEP, HTR_N_PERIOD, HTR_NB_PERIOD, HTR_DT_AMP);

    heater_pattern_values = generate_combined_profile(HTR_MODE, HTR_MAX_CUR, HTR_DT_TEMP, HTR_N_STEP, HTR_DT_STEP, HTR_N_PERIOD, HTR_NB_PERIOD, HTR_DT_AMP, &total_heater_pattern_points);

    if (heater_pattern_values == NULL)
    {
        printf("\nErro ao gerar o sinal. Verifique os parâmetros de entrada ou houve falha na alocação de memória.\n");
        // return 1;
    }

    for (size_t i = 0; i < total_heater_pattern_points; i++)
    {
        printf("step: %d, value: %.2f, state: %d, i: %d \n", heater_pattern_values[i].step, heater_pattern_values[i].value, heater_pattern_values[i].state, i);
    }

    /* Sensor Channel Configuration */
    cfg_sensSubsmp(TEMP1, TEMP1_SUBSMP, TEMP1_START_POS, TEMP1_SUSPEND_RUN);
    cfg_sensOvs(TEMP1, TEMP1_OVS_OFS, TEMP1_OVS_MULT, TEMP1_CIC_DEC);
    cfg_sensSubsmp(BARO, BARO_SUBSMP, BARO_START_POS, BARO_SUSPEND_RUN);
    cfg_sensOvs(BARO, BARO_OVS_OFS, BARO_OVS_MULT, BARO_CIC_DEC);
    cfg_sensSubsmp(WD, WD_SUBSMP, WD_START_POS, WD_SUSPEND_RUN);
    cfg_sensOvs(WD, WD_OVS_OFS, WD_OVS_MULT, WD_CIC_DEC);

    cfg_sensSubsmp(DIAG1, DIAG1_SUBSMP, DIAG1_START_POS, DIAG1_SUSPEND_RUN);
    cfg_sensOvs(DIAG1, DIAG1_OVS_OFS, DIAG1_OVS_MULT, DIAG1_CIC_DEC);

    /* Global variables */
    uint8_t int_status;
    uint8_t data_reg[128];
    uint16_t htr_power;
    uint8_t htr_loop_state;
    uint8_t htr_profile_state;
    uint8_t htr_vdd;
    uint32_t temp1_data;
    uint32_t baro_data;
    uint32_t wd_data;

    uint8_t htr_err;
    uint8_t rd_htr_data;

    uint32_t diag1_data; 
    uint32_t val_24_bits;
    int32_t diag1_data_aux;
    double diag1_data_signed;
    double htr_vdd_conv;
    uint32_t htr_vdd_lowest = 0xff;

    //==========================Enter in EXT_MODE and Disabel the UVLO ==========================
    unlockExtMode();
    coines_delay_msec(500);
    printf("Disabling UVLO!!!!!\n");
    uint8_t tst_h_config_data = 0x20; // 
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_TST_H_CONFIG, &tst_h_config_data, 1);

    coines_delay_msec(500);

    coines_delay_msec(1);
    uint8_t bdg_comm_data = 0xE8; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_ADDR0, &bdg_comm_data, 1);
    
    bdg_comm_data = 0x02; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_ADDR1, &bdg_comm_data, 1);

    bdg_comm_data = 0x31; // CMD to write
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_CMD, &bdg_comm_data, 1);
    
    bdg_comm_data = 0x00; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_DATA0, &bdg_comm_data, 1);

    bdg_comm_data = 0x20; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_DATA1, &bdg_comm_data, 1);
    bdg_comm_data = 0x00; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_DATA2, &bdg_comm_data, 1);
    // bdg_comm_data = 0x20; // MCU_CMD reg addr
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE2_BDG_DATA3, &bdg_comm_data, 1);
    
    coines_delay_msec(5);
    reg_addr = (SPI_PAGE2_BDG_STATUS | BMI2_SPI_RD_MASK);
    uint8_t bdg_status;
    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, &bdg_status, 1);

    printf("BDG_STATUS register value: %d\n", bdg_status);
    //============================================================================================================

#ifdef INFILE

    uint32_t time_us = 0;

    // oprn flash file
    char case_name[150];
    sprintf(case_name, "HTR_CL");
    printf("Test case: %s\n ", case_name);
    char file_name[200];
    sprintf(file_name, FILE_NAME);
    remove(file_name);
    FILE *fp = fopen(file_name, "w");
    // turn on green led once log file start
    coines_set_led(COINES_LED_GREEN, COINES_LED_STATE_ON);

    // trig free-run mode
    odrConfig(1, ODR_CFG);

    // read heater feature
    while (1)
    {
        if (interrupt1_status)
        {
            time_us = coines_get_realtime_usec();
            interrupt1_status = 0;

            (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_INT_STATUS_ADDR | BMI2_SPI_RD_MASK), &int_status, 1);
#ifdef READ_FIFO
            if ((int_status & EMU_INT_STATUS_FIFO_THS) == EMU_INT_STATUS_FIFO_THS)
            {
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_FIFO_DATA_ADDR | BMI2_SPI_RD_MASK), data_reg, 12 * fifo_thres);
                for (uint16_t i = 0; i < fifo_thres; i++)
                {
                    temp1_data = ((data_reg[i * 12 + 2] << 16) | (data_reg[i * 12 + 1] << 8) | data_reg[i * 12]) & 0xffffff;
                    baro_data = ((data_reg[i * 12 + 5] << 16) | (data_reg[i * 12 + 4] << 8) | data_reg[i * 12 + 3]) & 0xffffff;
                    wd_data = ((data_reg[i * 12 + 8] << 16) | (data_reg[i * 12 + 7] << 8) | data_reg[i * 12 + 6]) & 0xffffff;
                    htr_power = ((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) & 0x7ff;
                    htr_loop_state = (((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) >> 11) & 0b111;
                    htr_profile_state = (((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) >> 14) & 0b11;
                    htr_vdd = data_reg[i * 12 + 11];
                    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_CFG_ADDR | BMI2_SPI_RD_MASK), &rd_htr_data, 1);
                    if (rd_htr_data == 0)
                    {
                        break;
                    }
                    fprintf(fp, "%ld 0x%04x %d %d %d 0x%06x 0x%06x 0x%06x\n", time_us, htr_power, htr_loop_state, htr_profile_state, htr_vdd, temp1_data, baro_data, wd_data);
                }
                if (rd_htr_data == 0)
                {
                    break;
                }
            }
#else
            if ((int_status & EMU_INT_STATUS_DRDY_DATA_REG) == EMU_INT_STATUS_DRDY_DATA_REG)
            {
                cfg_data_ch_sel_legacy(HEATER);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];

                cfg_data_ch_sel_legacy(BARO);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                baro_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                cfg_data_ch_sel_legacy(WD);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                wd_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                cfg_data_ch_sel_legacy(TEMP1);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                temp1_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_CFG_ADDR | BMI2_SPI_RD_MASK), &rd_htr_data, 1);
                if (rd_htr_data == 0)
                {
                    break;
                }
                fprintf(fp, "%ld 0x%04x %d %d %d 0x%06x 0x%06x 0x%06x\n", time_us, htr_power, htr_loop_state, htr_profile_state, htr_vdd, temp1_data, baro_data, wd_data);
            }
#endif
        }
    }
    // check htr error
    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_ERROR_ADDR | BMI2_SPI_RD_MASK), &htr_err, 1);

    if (htr_err == 0)
        fprintf(fp, "Heater finished without errors \n");
    else
        fprintf(fp, "Heater finished with errors: 0x%02x \n", htr_err);

    odrConfig(0, ODR_CFG);
    (void)coines_set_shuttleboard_vdd_vddio_config(0, 0);
    coines_delay_msec(100);
    fclose(fp);

#else

    odrConfig(1, ODR_CFG);

    int loop = 0;

    while (1)
    {
        if (interrupt1_status)
        {
            interrupt1_status = 0;
             loop += 1;

            // read data
            (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_INT_STATUS_ADDR | BMI2_SPI_RD_MASK), &int_status, 1);
#ifdef READ_FIFO
            if ((int_status & EMU_INT_STATUS_FIFO_THS) == EMU_INT_STATUS_FIFO_THS)
            {
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_FIFO_DATA_ADDR | BMI2_SPI_RD_MASK), data_reg, 12 * fifo_thres);
                for (uint16_t i = 0; i < fifo_thres; i++)
                {
                    temp1_data = ((data_reg[i * 12 + 2] << 16) | (data_reg[i * 12 + 1] << 8) | data_reg[i * 12]) & 0xffffff;
                    baro_data = ((data_reg[i * 12 + 5] << 16) | (data_reg[i * 12 + 4] << 8) | data_reg[i * 12 + 3]) & 0xffffff;
                    wd_data = ((data_reg[i * 12 + 8] << 16) | (data_reg[i * 12 + 7] << 8) | data_reg[i * 12 + 6]) & 0xffffff;
                    htr_power = ((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) & 0x7ff;
                    htr_loop_state = (((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) >> 11) & 0b111;
                    htr_profile_state = (((data_reg[i * 12 + 10] << 8) | data_reg[i * 12 + 9]) >> 14) & 0b11;
                    htr_vdd = data_reg[i * 12 + 11];
                    if (htr_power == 0x77f)
                    {
                        break;
                    }
                    printf("htr_power = %u, htr_loop_state = %u, htr_profile_state = %u, htr_vdd = %d, temp1_data = %lu, baro_data = %lu, wd_data = %lu\n",
                           htr_power, htr_loop_state, htr_profile_state, htr_vdd, temp1_data, baro_data, wd_data);
                }
                if (htr_power == 0x77f)
                {
                    break;
                }
            }
#else
            if ((int_status & EMU_INT_STATUS_DRDY_DATA_REG) == EMU_INT_STATUS_DRDY_DATA_REG)
            {
                cfg_data_ch_sel_legacy(HEATER);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                htr_power = ((data_reg[1] << 8) | data_reg[0]) & 0x7ff;
                htr_loop_state = (((data_reg[1] << 8) | data_reg[0]) >> 11) & 0b111;
                htr_profile_state = (((data_reg[1] << 8) | data_reg[0]) >> 14) & 0b11;
                htr_vdd = data_reg[2];

                htr_vdd_conv = (double)htr_vdd/(1 << 6);

                cfg_data_ch_sel_legacy(BARO);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                baro_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                cfg_data_ch_sel_legacy(TEMP1);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                temp1_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                cfg_data_ch_sel_legacy(WD);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                wd_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;

                cfg_data_ch_sel_legacy(DIAG1);
                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | BMI2_SPI_RD_MASK), data_reg, 3);
                diag1_data = ((data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0]) & 0xffffff;
                // diag1_data_aux = (int32_t)diag1_data;
                if (diag1_data & 0x800000) {
                    val_24_bits = diag1_data & ((1U << 24) - 1);
                    diag1_data_aux = (int32_t)(val_24_bits << 8) >> 8;
                    diag1_data_signed = (double)diag1_data_aux / (1L << 21);
                } else {
                    diag1_data_signed = (double)diag1_data / (1L << 21);
                }

                if (htr_vdd < htr_vdd_lowest) htr_vdd_lowest = htr_vdd;

                printf("htr_power = %u, htr_loop_state = %u, htr_profile_state = %u, htr_vdd = %u, htr_vdd_conv = %.6f, DIAG1 = 0x%06x, VDD_DIAG = %.6f, temp1_data = %lu, baro_data = %lu, wd_data = %lu\n",
                       htr_power, htr_loop_state, htr_profile_state, htr_vdd, htr_vdd_conv, diag1_data, diag1_data_signed, temp1_data, baro_data, wd_data);

                // printf("htr_power = %u, htr_loop_state = %u, htr_profile_state = %u, htr_vdd = %u, temp1_data = %lu, baro_data = %lu, wd_data = %lu\n",
                //        htr_power, htr_loop_state, htr_profile_state, htr_vdd, temp1_data, baro_data, wd_data);

                // TODO reconsider the consideration to break on the code above
                bool temp1_comp_en = (comp & 1);
                char* report_check = check_heater_cl_step_report(temp1_data, temp1_comp_en); //Edson
                if (report_check != NULL) {
                    printf("%s \n", report_check);
                }
                free(report_check);

                (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_CFG_ADDR | BMI2_SPI_RD_MASK), &rd_htr_data, 1);
                if (rd_htr_data == 0)
                {
                    break;
                }
            }
#endif
        }
    }

    printf("Lowest VDD read: %d\n", htr_vdd_lowest);

    // Check that Heater finished w/o errorss
    (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_HEATER_ERROR_ADDR | BMI2_SPI_RD_MASK), &htr_err, 1);
    if (htr_err == 0)
    {
        printf("Heater finished without errors \n");
    }
    else
    {
        printf("Heater finished with errors: 0x%02x \n", htr_err);
    }

    // trig standby mode
    odrConfig(0, ODR_CFG);

#endif

    softReset_legacy();
    /* Coines interface reset */
    coines_soft_reset();
    coines_delay_msec(100);
    coines_close_comm_intf(comm_intf, NULL);
    return 0;
}
