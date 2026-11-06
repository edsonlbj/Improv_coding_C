#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include "generic_fpga.h"
#include "generic.h"
#include "check_function.h"

volatile uint8_t interrupt1_status = 0;

void interrupt1_callback(uint32_t param1, uint32_t param2)
{
    (void)param1;
    (void)param2;
    interrupt1_status = 1;
}

uint8_t check_data_vaild(uint32_t data, uint16_t meas_num, uint8_t sens_no)
{
    uint8_t err = 0;
    if (data == 0x7f7f7f)
    {
        printf("Error: Data of channel %d for measurement %d is not valid!\n", sens_no, meas_num);
        err = 1;
    }
    return err;
}

int32_t sign_extend_24_to_32(uint32_t val)
{
    uint32_t val_24_bit = val & 0x00FFFFFF;
    if (val_24_bit & 0x00800000)
    {
        return (int32_t)(val_24_bit | 0xFF000000);
    }
    else
    {
        return (int32_t)val_24_bit;
    }
}

uint8_t check_data_valid_stim(sensor_type_t sens_no, const stim_params_t *stim, uint32_t data)
{
    int32_t expected = 0;
    bool other_sens = false;

    switch (sens_no)
    {
    case SENSOR_TEMP1:
        // Formula: DATA_REG ~= stim_T_ofs/2^8
        expected = stim->stim_t_ofs >> 8;
        break;
    case SENSOR_TEMP2:
        // Formula: DATA_REG ~= stim_T_ofs/2^8
        expected = stim->stim_t_ofs >> 8;
        break;
    case SENSOR_TEMP3:
        // Formula: DATA_REG ~= stim_T_ofs/2^8
        expected = stim->stim_t_ofs >> 8;
        break;
    // case SENSOR_THERM:
    case SENSOR_BARO: // P1B
        // Formula: DATA_REG ~= stim_p1b_ofs/2^2
        expected = stim->stim_p1b_ofs >> 2;
        break;
    case SENSOR_EXTP: // P12B
        // Formula: DATA_REG ~= stim_p12b_ofs/2^6
        expected = stim->stim_p12b_ofs >> 6;
        break;
    case SENSOR_WD: // Water Detection
        // Formula: DATA_REG ~= stim_wd_ofs/2
        expected = stim->stim_wd_ofs >> 1;
        break;
    case SENSOR_DIAG1:
        expected = stim->stim_vdd_ofs >> 3;
        break;
    default:
        expected = (int32_t)(data & 0x00FFFFFF);
        other_sens = true;
    }

    int32_t actual_raw = sign_extend_24_to_32(data);


    int32_t diff = actual_raw - expected;

    if (other_sens)
    {
       if (actual_raw == sign_extend_24_to_32(0x7f7f7f)) 
        {
            printf("Error: Data of channel %d for measurement is not valid!\n", sens_no);
            return 1;
        }
        else {
             return 0;
        }
    }
    if (abs(diff) < DATA_TOLERANCE)
    {
        return 0;
    }
    else
    {
        printf("Error: Sensor %d - Measured 0x%06x (signed %ld), Expected 0x%06x (signed %ld) (Diff: %ld), Tolerance: %d\n",
               sens_no,
               (unsigned int)(actual_raw & 0x00FFFFFF), 
               actual_raw,  
               expected,
               diff,
               DATA_TOLERANCE);
        return 1;
    }
}

uint8_t check_data_invaild(uint32_t data, uint16_t meas_num, uint8_t sens_no)
{
    uint8_t err = 0;
    if (data != 0x7f7f7f)
    {
        printf("Error: Data of channel %d for measurement %d is valid!\n", sens_no, meas_num);
        err = 1;
    }
    return err;
}

uint8_t check_data_equal(uint32_t shdw_data, uint32_t fifo_data, uint16_t meas_num, uint8_t sens_no)
{
    uint8_t err = 0;
    if (shdw_data != fifo_data)
    {
        printf("Error: shdwdata and fifodata of channel %d for measurement %d is not equal!\n", sens_no, meas_num);
        err = 1;
    }
    return err;
}

void print_stim()
{

    uint8_t stim_data[4];

    printf("Programmed register value: \n");

    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P1B_AMP_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P1B_FREQ_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P1B_OFS_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P12B_AMP_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P12B_FREQ_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_P12B_OFS_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_T_AMP_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_T_FREQ_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_T_OFS_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_WD_AMP_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_WD_FREQ_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_WD_OFS_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_VDD_AMP_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_VDD_FREQ_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_5, STIM_VDD_OFS_ADDR | 0x80, stim_data, 4);
    printf("0x%02x ", (unsigned int)stim_data[0]);
    printf("0x%02x ", (unsigned int)stim_data[1]);
    printf("0x%02x ", (unsigned int)stim_data[2]);
    printf("0x%02x \n", (unsigned int)stim_data[3]);
    printf("----------------------------------------------------- \n");
}

/*
    Given the last shdw data, read the shadow and compare data
*/
uint8_t check_data_reg_async(uint8_t sens_no, uint32_t shdw_data[9])
{
    uint32_t shdw_data_async[9];
    for (sens_no = 0; sens_no < 9; sens_no++)
    {
        cfg_data_ch_sel_legacy(sens_no);
        uint8_t data_reg[3];
        (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_DATA_XLSB_ADDR | 0x80), data_reg, 3);
        shdw_data_async[sens_no] = (data_reg[2] << 16) | (data_reg[1] << 8) | data_reg[0];
    }
    printf("----------------------------  DATA Format -------------------------\n");
    printf("---- SHDW_DATA, SHDW_DATA_ASYNC -----------------------------------------\n");
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[0], (unsigned int)shdw_data_async[0]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[1], (unsigned int)shdw_data_async[1]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[2], (unsigned int)shdw_data_async[2]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[3], (unsigned int)shdw_data_async[3]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[4], (unsigned int)shdw_data_async[4]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[5], (unsigned int)shdw_data_async[5]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[6], (unsigned int)shdw_data_async[6]);
    printf("0x%06x, 0x%06x;   ", (unsigned int)shdw_data[7], (unsigned int)shdw_data_async[7]);
    printf("0x%06x, 0x%06x;   \n", (unsigned int)shdw_data[8], (unsigned int)shdw_data_async[8]);
    for (sens_no = 0; sens_no < 9; sens_no++)
    {
        if (shdw_data[sens_no] != shdw_data_async[sens_no])
        {
            printf("!!Error: shdwdata reg changed before the INTERRUPT \n");
            return 0;
        }
    }
    return 0;
}

uint8_t check_partial_frame_reading(uint8_t fifo_frame_size, uint8_t *output_frame_data)
{
    printf("CHECK: check partial reading.\n");
    uint8_t partial_read_size = 0;
    uint8_t frame_data[fifo_frame_size];

    partial_read_size = (rand() % (fifo_frame_size - 1)) + 1;
    if (partial_read_size > 3)
        partial_read_size = 3;
    if (partial_read_size >= fifo_frame_size)
        partial_read_size = fifo_frame_size - 1;
    if (partial_read_size == 0)
        partial_read_size = 1;

    uint8_t partial_frame_data[partial_read_size];
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_FIFO_DATA_ADDR | 0x80), partial_frame_data, partial_read_size);

    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_FIFO_DATA_ADDR | 0x80), frame_data, fifo_frame_size);

    for (uint8_t i = 0; i < partial_read_size; i++)
    {
        if (partial_frame_data[i] != frame_data[i])
        {
            printf("  ERROR: Partially read data DOES NOT match complete frame at index %d!\n", i);
            return 1;
        }
    }

    memcpy(output_frame_data, frame_data, fifo_frame_size);
    return 0;
}

uint8_t check_fifo_sens_consistency(
    uint32_t fifo_data[9][128],
    uint16_t current_loop,
    uint16_t *subsmp,
    uint8_t start_pos[9],
    int sensor_values[9])
{
    uint8_t err_flag_local = 0;
    uint8_t sens_no;
    uint8_t active_sensors = 0;
    for (sens_no = 0; sens_no < 9; sens_no++)
    {
        if (sensor_values[sens_no] == 1)
        {
            printf("Sens %d           ", sens_no);
            active_sensors++;
        }
    }
    printf("\n");

    for (sens_no = 0; sens_no < active_sensors; sens_no++)
    {
        printf("0x%06x         ", (unsigned int)fifo_data[sens_no][current_loop]);
    }
    printf("\n");

    // check data
    uint8_t sens_on_no = 0;
    for (sens_no = 0; sens_no < 9; sens_no++)
    {
        if (subsmp[sens_no] > 0)
        {
            if (current_loop < start_pos[sens_no])
            {
                err_flag_local = check_data_invaild(fifo_data[sens_on_no][current_loop], current_loop, sens_no);
            }
            else
            {
                if ((current_loop - start_pos[sens_no]) % subsmp[sens_no] == 0)
                {
                    // err_flag_local = check_data_vaild(fifo_data[sens_on_no][current_loop], current_loop, sens_no);
                }
                else
                {
                    err_flag_local = check_data_invaild(fifo_data[sens_on_no][current_loop], current_loop, sens_no);
                }
            }
            sens_on_no++;
        };
    }
    return err_flag_local;
};

uint8_t check_FIFO_depth(uint8_t fifo_depth_calc)
{
    uint8_t fifo_depth = get_fifoDepth_legacy();

    if (fifo_depth == fifo_depth_calc)
    {
        return 0;
    }
    else
    {
        return 1;
    }
};

uint8_t check_FIFO_frame_size(uint8_t fifo_frame_size_calc)
{
    uint8_t fifo_frame_size_ = get_fifoFrameSize_legacy();

    if (fifo_frame_size_ == fifo_frame_size_calc)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t check_interruptStatus()
{
    if (get_interruptStatus_legacy() == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

uint8_t check_FIFO_count(uint16_t level)
{
    if (get_fifoCount_legacy() == level)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void get_otp_comp(uint8_t cfgParam0, char *coeff)
{
    uint8_t cfgParam1 = 0x02;
    uint8_t cfgStatus0 = 0;
    uint8_t cfgStatus1 = 0;
    uint8_t cfgStatus2 = 0;
    uint8_t cfgStatus3 = 0;
    uint8_t cfgCmd = 0x09;
    unsigned char bit_0_mask = 0x01;

    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_CFG_PARAM_1_ADDR, &cfgParam1, 1);
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_CFG_PARAM_0_ADDR, &cfgParam0, 1); // COMP_COEFF_TEMP_0 KD0
    coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, SPI_PAGE0_CFG_CMD_ADDR, &cfgCmd, 1);        // COMP_PARAM_READ + TRIG

    while ((cfgCmd & bit_0_mask) == 1)
    {
        coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_CFG_CMD_ADDR | 0x80), &cfgCmd, 1);
    }

    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_CFG_STATUS_0_ADDR | 0x80), &cfgStatus0, 1);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_CFG_STATUS_1_ADDR | 0x80), &cfgStatus1, 1);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_CFG_STATUS_2_ADDR | 0x80), &cfgStatus2, 1);
    coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (SPI_PAGE0_CFG_STATUS_3_ADDR | 0x80), &cfgStatus3, 1);

    printf("%s \n", coeff);
    printf("%x  ", cfgStatus0);
    printf("%x  ", cfgStatus1);
    printf("%x  ", cfgStatus2);
    printf("%x  \n", cfgStatus3);
}

void get_all_otp_comp()
{
    get_otp_comp(0x00, "COMP_COEFF_TEMP_0");
    get_otp_comp(0x01, "COMP_COEFF_TEMP_1");
    get_otp_comp(0x02, "COMP_COEFF_TEMP_2");
    get_otp_comp(0x03, "COMP_COEFF_TEMP_3");
    get_otp_comp(0x04, "COMP_COEFF_TEMP_4");
    get_otp_comp(0x05, "COMP_COEFF_TEMP_5");
    get_otp_comp(0x08, "COMP_COEFF_VDD_0");
    get_otp_comp(0x09, "COMP_COEFF_VDD_1");
    get_otp_comp(0x0a, "COMP_COEFF_THERM_0");
    get_otp_comp(0x0b, "COMP_COEFF_THERM_1");
}