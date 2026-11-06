#include "generic.h"
#include "coines.h"
#include <math.h>

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include "stdio.h"
#endif


/***************************************************************************/
/*!              Static Variable
 ****************************************************************************/
volatile uint32_t DEF_USR_TRIM_0;
volatile uint32_t DEF_USR_TRIM_1;
volatile uint32_t DEF_USR_TRIM_2;
volatile uint32_t DEF_USR_TRIM_3;
volatile uint32_t DEF_USR_TRIM_4;
volatile uint32_t DEF_USR_TRIM_5;

// default hif setting
HifConfig def_hif = {.type = SPI, .cfg.spi={COINES_SPI_MODE0, SPI_SPEED_8MHZ}};

/******************************************************************************/
/*!         Local Function Prototypes
******************************************************************************/

//=======================================================
// Hif Interface 
//=======================================================

void config_hif_bus(const HifConfig *hif)
{
    if (hif->type == SPI || hif->type==SPI_03 || hif->type==SPI_12)
    {
        /* SPI config */
        coines_config_spi_bus(COINES_SPI_BUS_0, hif->cfg.spi.speed, hif->cfg.spi.mode);
        /* Make CSB pin HIGH */
        coines_set_pin_config(COINES_SHUTTLE_PIN_21, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
        coines_delay_msec(100);
        #if DEBUG == true
            printf("HIF_MODE: SPI; ");
            switch(hif->cfg.spi.speed){
                case SPI_SPEED_8MHZ  : printf("SPI_SPEED: 8MHZ; "); break;
                case SPI_SPEED_4MHZ  : printf("SPI_SPEED: 4MHZ; "); break;
                case SPI_SPEED_2MHZ  : printf("SPI_SPEED: 2MHZ; "); break;
                case SPI_SPEED_1MHZ  : printf("SPI_SPEED: 1MHZ; "); break;
                case SPI_SPEED_500KHZ: printf("SPI_SPEED: 500KHZ; "); break;
                case SPI_SPEED_250KHZ: printf("SPI_SPEED: 250KHZ; "); break;
                default              : printf("SPI_SPEED: Undefined value; ");
            }
            printf("SPI_MODE: %d\n", hif->cfg.spi.mode);
        #endif
        /* Dummy Byte */
        uint8_t dummy_byte;
        (void)coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, 0x00, &dummy_byte, 1);
        coines_delay_usec(450);
    } 
    else 
    {
        /* I2C config */
        coines_config_i2c_bus(COINES_I2C_BUS_0, hif->cfg.i2c.mode);
        /* Set SDO pin */
        if (hif->cfg.i2c.dev_adddr == 0x77) {
            coines_set_pin_config(COINES_SHUTTLE_PIN_SDO, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);
        } else {
            coines_set_pin_config(COINES_SHUTTLE_PIN_SDO, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_LOW);
        }
        coines_delay_msec(100);
        #if DEBUG == true
            printf("HIF_MODE: I2C; ");
            switch(hif->cfg.i2c.mode){
                case COINES_I2C_STANDARD_MODE: printf("I2C_SPEED: standard mode; "); break;
                case COINES_I2C_FAST_MODE    : printf("I2C_SPEED: fast mode; "); break;
                case COINES_I2C_SPEED_3_4_MHZ: printf("I2C_SPEED: 3.4 MHz; "); break;
                case COINES_I2C_SPEED_1_7_MHZ: printf("I2C_SPEED: 1.7 MHz; "); break;
                default                      : printf("I2C_SPEED: Undefined value; ");
            }
            printf("I2C Device Addr: 0x%02x\n", hif->cfg.i2c.dev_adddr);
        #endif
        /* Dummy Byte */
        uint8_t dummy_byte;
        (void)coines_read_i2c(COINES_I2C_BUS_0, hif->cfg.i2c.dev_adddr, 0x00, &dummy_byte, 1);
        coines_delay_usec(450);
    }
}

void hif_write(const HifConfig *hif, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    if (hif->type == SPI || hif->type==SPI_03 || hif->type==SPI_12)
        coines_write_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, reg_addr, reg_data, count);
    else 
        coines_write_i2c(COINES_I2C_BUS_0, hif->cfg.i2c.dev_adddr, reg_addr, reg_data, count);
}

void hif_read(const HifConfig *hif, uint8_t reg_addr, uint8_t *reg_data, uint16_t count)
{
    if (hif->type == SPI || hif->type==SPI_03 || hif->type==SPI_12)
        coines_read_spi(COINES_SPI_BUS_0, COINES_MINI_SHUTTLE_PIN_2_1, (reg_addr | SPI_RD_MASK), reg_data, count);
    else 
        coines_read_i2c(COINES_I2C_BUS_0, hif->cfg.i2c.dev_adddr, reg_addr, reg_data, count);
}

//=======================================================
// Measurement
//=======================================================

void config_sensors(const HifConfig *hif, const SensorConfig *sensor)
{
    uint8_t cfgSubsmp, cfgOvs;
    uint8_t comp = 0;
    for (uint8_t i = 0; i < MAX_SENSOR_NUM; i++) {
        cfgSubsmp = ((sensor->ch_cfg[i].suspend_run & 0x1) << 7) |
                    ((sensor->ch_cfg[i].start_pos   & 0x7) << 4) |
                    ( sensor->ch_cfg[i].subsmp      & 0xf);
        cfgOvs    = ((sensor->ch_cfg[i].cic_dec     & 0x3) << 6) |
                    ((sensor->ch_cfg[i].ovs_mult    & 0x3) << 4) |
                    ( sensor->ch_cfg[i].ovs_ofs     & 0xf);
        hif_write(hif, SPI_PAGE0_TEMP1_SUBSMP_ADDR+i*2, &cfgSubsmp, 1);
        hif_write(hif, SPI_PAGE0_TEMP1_OVS_ADDR+i*2, &cfgOvs, 1);
        if (sensor->ch_cfg[i].comp_en == 1) {
            comp = comp | (1 << i);
        } 
        #if DEBUG == true
            uint8_t rdata;
            hif_read(hif, SPI_PAGE0_TEMP1_SUBSMP_ADDR+i*2, &rdata, 1);
            printf("Sensor %d SUBSMP is 0x%02x \n", i, rdata);
	        if(rdata != cfgSubsmp) {
	        	printf("Error: Sensor %d SUBSMP is not set properly! \n", i);
	        }
            hif_read(hif, SPI_PAGE0_TEMP1_OVS_ADDR+i*2, &rdata, 1);
            printf("Sensor %d OVS is 0x%02x \n", i, rdata);
	        if(rdata != cfgOvs) {
	        	printf("Error: Sensor %d OVS is not set properly! \n", i);
	        }
        #endif
    }
    hif_write(hif, SPI_PAGE0_COMP_ADDR, &comp, 1);
    #if DEBUG == true
        uint8_t rdata;
        hif_read(hif, SPI_PAGE0_COMP_ADDR, &rdata, 1);
        printf("COMP is 0x%02x \n", rdata);
	    if(rdata != comp) {
	    	printf("Error: COMP is not set properly! \n");
	    }
    #endif

}

void config_diags(const HifConfig *hif, const DiagConfig *diag)
{
    uint8_t diag_reg[12];
    // diag1
    diag_reg[0] = ((diag->high_clk_f_en[0]  & 0x1) << 7) |
                  ((diag->leakage_lna_en[0] & 0x1) << 6) |
                  ((~(diag->ch_cfg[0].comp_en) & 0x1) << 5) |
                  diag->meas_sel[0];  
    diag_reg[1] = ((diag->ch_cfg[0].cic_dec     & 0x3) << 6) |
                  ((diag->ch_cfg[0].ovs_mult    & 0x3) << 4) |
                  ( diag->ch_cfg[0].ovs_ofs     & 0xf);
    diag_reg[2] = ((diag->ch_cfg[0].suspend_run & 0x1) << 7) |
                  ((diag->ch_cfg[0].start_pos   & 0x7) << 4) |
                  ( diag->ch_cfg[0].subsmp      & 0xf);
    diag_reg[3] = diag->c2v_cfg1[0];
    diag_reg[4] = diag->c2v_cfg2[0];
    diag_reg[5] = diag->c2v_cfg3[0];
    // diag2
    diag_reg[6] = ((diag->high_clk_f_en[1]  & 0x1) << 7) |
                  ((diag->leakage_lna_en[1] & 0x1) << 6) |
                  ((~(diag->ch_cfg[1].comp_en) & 0x1) << 5) |
                 diag->meas_sel[1];
    diag_reg[7] = ((diag->ch_cfg[1].cic_dec     & 0x3) << 6) |
                  ((diag->ch_cfg[1].ovs_mult    & 0x3) << 4) |
                  ( diag->ch_cfg[1].ovs_ofs     & 0xf);
    diag_reg[8] = ((diag->ch_cfg[1].suspend_run & 0x1) << 7) |
                  ((diag->ch_cfg[1].start_pos   & 0x7) << 4) |
                  ( diag->ch_cfg[1].subsmp      & 0xf);
    diag_reg[9] = diag->c2v_cfg1[1];
    diag_reg[10]= diag->c2v_cfg2[1];
    diag_reg[11]= diag->c2v_cfg3[1];

    if (hif->type == SPI || hif->type==SPI_03 || hif->type==SPI_12){
        uint8_t wdata_buff[23];
        wdata_buff[0] = diag_reg[0];
        for (int i = 1; i < 12; i++) {
            wdata_buff[i*2-1] = SPI_PAGE0_DIAG1_CFG_ADDR+i;
            wdata_buff[i*2]   = diag_reg[i];
        }
        hif_write(hif, SPI_PAGE0_DIAG1_CFG_ADDR, wdata_buff, 23);
    } else {
        hif_write(hif, SPI_PAGE0_DIAG1_CFG_ADDR, diag_reg, 12);
    }

    #if DEBUG == true
        uint8_t rdata;
        hif_read(hif, SPI_PAGE0_DIAG1_CFG_ADDR, &rdata, 1);
        printf("DIAG1_CFG is 0x%02x \n", rdata);
        hif_read(hif, SPI_PAGE0_DIAG2_CFG_ADDR, &rdata, 1);
        printf("DIAG2_CFG is 0x%02x \n", rdata);
    #endif
    
}

void cfg_data_ch_sel(const HifConfig *hif, uint8_t sens_no)
{
	uint8_t data_ch_sel = 0;
	switch (sens_no)
	{
		case TEMP1:
			data_ch_sel = 0x00;
			break;
		case TEMP2:
			data_ch_sel = 0x10;
			break;
		case TEMP3:
			data_ch_sel = 0x20;
			break;
		case THERM:
			data_ch_sel = 0x30;
			break;
		case BARO:
			data_ch_sel = 0x40;
			break;
		case EXTP:
			data_ch_sel = 0x50;
			break;
		case WD:
			data_ch_sel = 0x60;
			break;
		case DIAG1:
			data_ch_sel = 0x70;
			break;
		case DIAG2:
			data_ch_sel = 0x80;
			break;
		case HEATER:
			data_ch_sel = 0x90;
			break;
		case FLAG_MEAS:
			data_ch_sel = 0xA0;
			break;
		default:
		    data_ch_sel = 0x00;
			break;
	}
    hif_write(hif, SPI_PAGE0_DATA_CH_SEL_ADDR, &data_ch_sel, 1);
}


void config_odr(const HifConfig *hif, uint8_t odr_cfg, uint8_t odr_trig)
{
    uint8_t odr_config = ((odr_cfg & 0x7f) << 1) | (odr_trig & 0x01) ;
    hif_write(hif, SPI_PAGE0_ODR_CONFIG_ADDR, &odr_config, 1);
}

uint32_t get_odr_period_us(uint8_t odr_cfg){
    uint32_t period;
    switch(odr_cfg){
        case 9:
            period = 1066667;
            break;
        case 15:
            period = 133333;
            break;
        case 28:
            period = 66667;
            break;
        case 41:
            period = 33333;
            break;
        case 54:
            period = 16667;
            break;
        case 67:
            period = 8333;
            break;
        case 80:
            period = 4167;
            break;
        case 93:
            period = 2083;
            break;
        case 106:
            period = 1042;
            break;
        default:
            period = 33333;
            printf("Undefined period for odr_cfg %d\n", odr_cfg);
            break;
    }
    return period;
}

void get_odr_freq(uint8_t odr_cfg, double *exp_odr)
{
	if (odr_cfg == 0 || odr_cfg == 127)
	{
		printf("Current cfg is not free-run mode.\n");
	}
	else if(odr_cfg >= 1 && odr_cfg <= 14) 
	{
		*exp_odr = (double)24960/32/104/pow(2, (15-odr_cfg)/2.0);
	}
	else if (odr_cfg >= 15 && odr_cfg <= 27) 
	{
		*exp_odr = (double)24960/128/(41-odr_cfg);
	}
	else if (odr_cfg >= 28 && odr_cfg <= 40)
	{
		*exp_odr = (double)24960/64/(54-odr_cfg);
	}
	else if (odr_cfg >= 41 && odr_cfg <= 53) 
	{
		*exp_odr = (double)24960/32/(67-odr_cfg);
	}
	else if (odr_cfg >= 51 && odr_cfg <= 66) 
	{
		*exp_odr = (double)24960/16/(80-odr_cfg);
	}
	else if (odr_cfg >= 67 && odr_cfg <= 79) 
	{
		*exp_odr = (double)24960/8/(93-odr_cfg);
	}
	else if (odr_cfg >= 80 && odr_cfg <= 92) 
	{
		*exp_odr = (double)24960/4/(106-odr_cfg);
	}
	else if (odr_cfg >= 93 && odr_cfg <= 105) 
	{
		*exp_odr = (double)24960/2/(119-odr_cfg);
	} 
	else 
	{
		*exp_odr = (double)24960/1/(132-odr_cfg);
	}
}

//=======================================================
// FIFO
//=======================================================

uint8_t config_fifo(const HifConfig *hif, const FIFOConfig *fifo)
{
    uint8_t fifo_config = fifo->mode | fifo->threshold;
    uint8_t fifo_frame_cfg_lsb = (fifo->frame_cfg_lsb) & 0xff;
    uint8_t fifo_frame_cfg_msb = (fifo->frame_cfg_msb) & 0x07;
	hif_write(hif, SPI_PAGE0_FIFO_CONFIG_ADDR, &fifo_config, 1);
    hif_write(hif, SPI_PAGE0_FIFO_FRAME_CFG_LSB_ADDR, &fifo_frame_cfg_lsb, 1);
    hif_write(hif, SPI_PAGE0_FIFO_FRAME_CFG_MSB_ADDR, &fifo_frame_cfg_msb, 1);

    uint8_t frame_size = 0;
    for (uint8_t i = 0; i < 8; i++) { 
        if ((fifo_frame_cfg_lsb >> i) & 0b1) {
            frame_size = frame_size + 3;
        }
    }
    for (uint8_t i = 0; i < 3; i++) { 
        if ((fifo_frame_cfg_msb >> i) & 0b1) {
            frame_size = frame_size + 3;
        }
    }

    #if DEBUG == true
        uint8_t rdata;
        hif_read(hif, SPI_PAGE0_FIFO_CONFIG_ADDR, &rdata, 1);
        printf("FIFO_CONFIG is 0x%02x \n", rdata);
	    if(rdata != fifo_config) {
	    	printf("Error: FIFO_CONFIG is not set properly! \n");
	    }
        hif_read(hif, SPI_PAGE0_FIFO_FRAME_CFG_LSB_ADDR, &rdata, 1);
        printf("FIFO_FRAME_CFG_LSB is 0x%02x \n", rdata);
	    if(rdata != fifo_frame_cfg_lsb) {
	    	printf("Error: FIFO_FRAME_CFG_LSB is not set properly! \n");
	    }
        hif_read(hif, SPI_PAGE0_FIFO_FRAME_CFG_MSB_ADDR, &rdata, 1);
        printf("FIFO_FRAME_CFG_MSB is 0x%02x \n", rdata);
	    if(rdata != fifo_frame_cfg_msb) {
	    	printf("Error: FIFO_FRAME_CFG_MSB is not set properly! \n");
	    }
    #endif

    return frame_size;
}

uint8_t get_fifoDepth(const HifConfig *hif)
{
	uint8_t rdata = 0;
    hif_read(hif, SPI_PAGE0_FIFO_DEPTH_ADDR, &rdata, 1);	
	return rdata;
}
uint8_t get_fifoFrameSize(const HifConfig *hif)
{
    uint8_t rdata = 0;
    hif_read(hif, SPI_PAGE0_FIFO_FRAME_SIZE_ADDR, &rdata, 1);	
	return rdata;
}
uint8_t get_fifoCount(const HifConfig *hif)
{
	uint8_t rdata = 0;
    hif_read(hif, SPI_PAGE0_FIFO_COUNT_ADDR, &rdata, 1);	
	return rdata;
}

//=======================================================
// Interrupt
//=======================================================

void config_interrupt(const HifConfig *hif, const InterruptConfig *interrupt)
{
    uint8_t int_config = interrupt->pol | interrupt->mode;
    uint8_t int_source = interrupt->source;
    hif_write(hif, SPI_PAGE0_INT_CONFIG_ADDR, &int_config, 1);
    hif_write(hif, SPI_PAGE0_INT_SOURCE_ADDR, &int_source, 1);

    #if DEBUG == true
        uint8_t rdata;
        hif_read(hif, SPI_PAGE0_INT_CONFIG_ADDR, &rdata, 1);
        printf("INT_CONFIG is 0x%02x \n", rdata);
	    if(rdata != int_config) {
	    	printf("Error: INT_CONFIG is not set properly! \n");
	    }
        hif_read(hif, SPI_PAGE0_INT_SOURCE_ADDR, &rdata, 1);
        printf("INT_SOURCE is 0x%02x \n", rdata);
	    if(rdata != int_source) {
	    	printf("Error: INT_SOURCE is not set properly! \n");
	    }
    #endif
}

uint8_t get_interruptStatus(const HifConfig *hif)
{
	uint8_t rdata = 0;
	hif_read(hif, SPI_PAGE0_INT_STATUS_ADDR, &rdata, 1);	
	return rdata;
}

//=======================================================
// MTP operation
//=======================================================

void mtp_prog(const HifConfig *hif, uint8_t addr, uint8_t key, uint32_t mtp_wr_data)
{
    uint8_t cfg_param_data;
    hif_write(hif, SPI_PAGE0_CFG_PARAM_0_ADDR, &addr, 1);
    hif_write(hif, SPI_PAGE0_CFG_PARAM_1_ADDR, &key, 1);
    cfg_param_data = mtp_wr_data & 0xff;
    hif_write(hif, SPI_PAGE0_CFG_PARAM_4_ADDR, &cfg_param_data, 1);
    cfg_param_data = (mtp_wr_data >> 8) & 0xff;
    hif_write(hif, SPI_PAGE0_CFG_PARAM_5_ADDR, &cfg_param_data, 1);
    cfg_param_data = (mtp_wr_data >> 16) & 0xff;
    hif_write(hif, SPI_PAGE0_CFG_PARAM_6_ADDR, &cfg_param_data, 1);
    cfg_param_data = (mtp_wr_data >> 24) & 0xff;
    hif_write(hif, SPI_PAGE0_CFG_PARAM_7_ADDR, &cfg_param_data, 1);

    uint8_t cfg_cmd = 0x05;
    hif_write(hif, SPI_PAGE0_CFG_CMD_ADDR, &cfg_cmd, 1);
    coines_delay_msec(20);
}

void mtp_read(const HifConfig *hif, uint8_t addr, uint32_t *mtp_rd_data)
{
    hif_write(hif, SPI_PAGE0_CFG_PARAM_0_ADDR, &addr, 1);

    uint8_t cfg_cmd = 0x03;
    hif_write(hif, SPI_PAGE0_CFG_CMD_ADDR, &cfg_cmd, 1);
    coines_delay_msec(5);

    uint32_t mtp_data = 0;
    uint8_t cfg_status;
    hif_read(hif, SPI_PAGE0_CFG_STATUS_3_ADDR, &cfg_status, 1);
    mtp_data = (mtp_data << 8) | cfg_status;
    hif_read(hif, SPI_PAGE0_CFG_STATUS_2_ADDR, &cfg_status, 1);
    mtp_data = (mtp_data << 8) | cfg_status;
    hif_read(hif, SPI_PAGE0_CFG_STATUS_1_ADDR, &cfg_status, 1);
    mtp_data = (mtp_data << 8) | cfg_status;
    hif_read(hif, SPI_PAGE0_CFG_STATUS_0_ADDR, &cfg_status, 1);
    mtp_data = (mtp_data << 8) | cfg_status;
    *mtp_rd_data = mtp_data;
}

void store_mtp_init_data() 
{
    // Assume Default hif_mode = AUTO; read default MTP data
    config_hif_bus(&def_hif);
    uint32_t mtp_data;
    mtp_read(&def_hif, 0x0, &mtp_data);
    DEF_USR_TRIM_0 = mtp_data;
    mtp_read(&def_hif, 0x1, &mtp_data);
    DEF_USR_TRIM_1 = mtp_data;
    mtp_read(&def_hif, 0x2, &mtp_data);
    DEF_USR_TRIM_2 = mtp_data;
    mtp_read(&def_hif, 0x3, &mtp_data);
    DEF_USR_TRIM_3 = mtp_data;
    mtp_read(&def_hif, 0x4, &mtp_data);
    DEF_USR_TRIM_4 = mtp_data;
    mtp_read(&def_hif, 0x5, &mtp_data);
    DEF_USR_TRIM_5 = mtp_data;

    uint8_t error_status = 0;
    hif_read(&def_hif, SPI_PAGE0_ERROR_STATUS_ADDR, &error_status, 1);
    if(error_status)
        printf("ERROR_STATUS = 0x%02x\n", error_status);
    softReset(&def_hif);
}

void load_mtp_init_data(const HifConfig *hif)
{
    mtp_prog(hif, 0x0, MTP_KEY, DEF_USR_TRIM_0);
    mtp_prog(hif, 0x1, MTP_KEY, DEF_USR_TRIM_1);
    mtp_prog(hif, 0x2, MTP_KEY, DEF_USR_TRIM_2);
    mtp_prog(hif, 0x3, MTP_KEY, DEF_USR_TRIM_3);
    mtp_prog(hif, 0x4, MTP_KEY, DEF_USR_TRIM_4);
    mtp_prog(hif, 0x5, MTP_KEY, DEF_USR_TRIM_5);

    uint8_t error_status = 0;
    hif_read(hif, SPI_PAGE0_ERROR_STATUS_ADDR, &error_status, 1);
    if(error_status)
        printf("ERROR_STATUS = 0x%02x\n", error_status);
}

//=======================================================
// Miscellaneous
//=======================================================

void softReset(const HifConfig *hif)
{
    uint8_t data = 0xDE;
	hif_write(hif, SPI_PAGE0_CMD_ADDR, &data, 1);
	coines_delay_msec(500);

    if(hif->type == SPI || hif->type == SPI_03 || hif->type == SPI_12)
        coines_deconfig_spi_bus(COINES_SPI_BUS_0);
    else
        coines_deconfig_i2c_bus(COINES_I2C_BUS_0);
    coines_delay_msec(10);
}

uint8_t check_pin_value(enum coines_multi_io_pin pin_number, uint8_t exp_value)
{
	enum coines_pin_value pin_value;
	coines_get_pin_config(pin_number, NULL, &pin_value);
    if (pin_value != exp_value) {
        #if DEBUG == true
        printf("Error: INT pin_value is %d, expected value is %d\n", pin_value, exp_value);
        #endif
        return 1;
    }
    return 0;
}









