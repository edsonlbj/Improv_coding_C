#ifndef _HORIZON_ADDRMAP_H
#define _HORIZON_ADDRMAP_H

//-----------------------------------------------------------------------------
// Address block: SPI_PAGE0
//-----------------------------------------------------------------------------
#define SPI_PAGE0_BASE_ADDR                   0x000
#define SPI_PAGE0_LENGTH                      0x080
#define SPI_PAGE0_LAST_ADDR                   0x07F
#define SPI_PAGE0_MANU_ID_ADDR                (SPI_PAGE0_BASE_ADDR + 0x000)
#define SPI_PAGE0_CHIP_ID_ADDR                (SPI_PAGE0_BASE_ADDR + 0x001)
#define SPI_PAGE0_SI_ID_ADDR                  (SPI_PAGE0_BASE_ADDR + 0x002)
#define SPI_PAGE0_PKG_ID_ADDR                 (SPI_PAGE0_BASE_ADDR + 0x003)
#define SPI_PAGE0_TRIM_ID_ADDR                (SPI_PAGE0_BASE_ADDR + 0x004)
#define SPI_PAGE0_SUBCON_BLD_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x005)
#define SPI_PAGE0_SUBCON_BOM_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x006)
#define SPI_PAGE0_MPN_ID_ADDR                 (SPI_PAGE0_BASE_ADDR + 0x007)
#define SPI_PAGE0_ASIC_LOT_0_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x008)
#define SPI_PAGE0_ASIC_LOT_1_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x009)
#define SPI_PAGE0_ASIC_LOT_2_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00A)
#define SPI_PAGE0_ASIC_LOT_3_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00B)
#define SPI_PAGE0_ASIC_LOT_4_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00C)
#define SPI_PAGE0_ASIC_LOT_5_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00D)
#define SPI_PAGE0_ASIC_WAFER_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00E)
#define SPI_PAGE0_ASIC_DIE_Y_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x00F)
#define SPI_PAGE0_ASIC_DIE_X_ID_ADDR          (SPI_PAGE0_BASE_ADDR + 0x010)
#define SPI_PAGE0_DATA_CH_SEL_ADDR            (SPI_PAGE0_BASE_ADDR + 0x01A)
#define SPI_PAGE0_DATA_XLSB_ADDR              (SPI_PAGE0_BASE_ADDR + 0x01B)
#define SPI_PAGE0_DATA_LSB_ADDR               (SPI_PAGE0_BASE_ADDR + 0x01C)
#define SPI_PAGE0_DATA_MSB_ADDR               (SPI_PAGE0_BASE_ADDR + 0x01D)
#define SPI_PAGE0_INT_SHORT_DET_RES_ADDR      (SPI_PAGE0_BASE_ADDR + 0x01E)
#define SPI_PAGE0_ERROR_STATUS_ADDR           (SPI_PAGE0_BASE_ADDR + 0x024)
#define SPI_PAGE0_FIFO_DEPTH_ADDR             (SPI_PAGE0_BASE_ADDR + 0x025)
#define SPI_PAGE0_FIFO_FRAME_SIZE_ADDR        (SPI_PAGE0_BASE_ADDR + 0x026)
#define SPI_PAGE0_INT_STATUS_ADDR             (SPI_PAGE0_BASE_ADDR + 0x027)
#define SPI_PAGE0_FIFO_COUNT_ADDR             (SPI_PAGE0_BASE_ADDR + 0x028)
#define SPI_PAGE0_FIFO_DATA_ADDR              (SPI_PAGE0_BASE_ADDR + 0x029)
#define SPI_PAGE0_CHIP_STATUS_ADDR            (SPI_PAGE0_BASE_ADDR + 0x030)
#define SPI_PAGE0_INT_CONFIG_ADDR             (SPI_PAGE0_BASE_ADDR + 0x031)
#define SPI_PAGE0_INT_SOURCE_ADDR             (SPI_PAGE0_BASE_ADDR + 0x032)
#define SPI_PAGE0_DRIVE_CONFIG_INT_ADDR       (SPI_PAGE0_BASE_ADDR + 0x034)
#define SPI_PAGE0_DRIVE_CONFIG_SDA_SDO_ADDR   (SPI_PAGE0_BASE_ADDR + 0x036)
#define SPI_PAGE0_FIFO_CONFIG_ADDR            (SPI_PAGE0_BASE_ADDR + 0x038)
#define SPI_PAGE0_FIFO_FRAME_CFG_MSB_ADDR     (SPI_PAGE0_BASE_ADDR + 0x039)
#define SPI_PAGE0_FIFO_FRAME_CFG_LSB_ADDR     (SPI_PAGE0_BASE_ADDR + 0x03A)
#define SPI_PAGE0_WD_UTH_MSB_ADDR             (SPI_PAGE0_BASE_ADDR + 0x03B)
#define SPI_PAGE0_WD_UTH_LSB_ADDR             (SPI_PAGE0_BASE_ADDR + 0x03C)
#define SPI_PAGE0_WD_LTH_MSB_ADDR             (SPI_PAGE0_BASE_ADDR + 0x03D)
#define SPI_PAGE0_WD_LTH_LSB_ADDR             (SPI_PAGE0_BASE_ADDR + 0x03E)
#define SPI_PAGE0_CFG_PARAM_0_ADDR            (SPI_PAGE0_BASE_ADDR + 0x040)
#define SPI_PAGE0_CFG_PARAM_1_ADDR            (SPI_PAGE0_BASE_ADDR + 0x041)
#define SPI_PAGE0_CFG_PARAM_2_ADDR            (SPI_PAGE0_BASE_ADDR + 0x042)
#define SPI_PAGE0_CFG_PARAM_3_ADDR            (SPI_PAGE0_BASE_ADDR + 0x043)
#define SPI_PAGE0_CFG_PARAM_4_ADDR            (SPI_PAGE0_BASE_ADDR + 0x044)
#define SPI_PAGE0_CFG_PARAM_5_ADDR            (SPI_PAGE0_BASE_ADDR + 0x045)
#define SPI_PAGE0_CFG_PARAM_6_ADDR            (SPI_PAGE0_BASE_ADDR + 0x046)
#define SPI_PAGE0_CFG_PARAM_7_ADDR            (SPI_PAGE0_BASE_ADDR + 0x047)
#define SPI_PAGE0_CFG_STATUS_0_ADDR           (SPI_PAGE0_BASE_ADDR + 0x048)
#define SPI_PAGE0_CFG_STATUS_1_ADDR           (SPI_PAGE0_BASE_ADDR + 0x049)
#define SPI_PAGE0_CFG_STATUS_2_ADDR           (SPI_PAGE0_BASE_ADDR + 0x04A)
#define SPI_PAGE0_CFG_STATUS_3_ADDR           (SPI_PAGE0_BASE_ADDR + 0x04B)
#define SPI_PAGE0_CFG_CMD_ADDR                (SPI_PAGE0_BASE_ADDR + 0x04E)
#define SPI_PAGE0_TEMP1_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x050)
#define SPI_PAGE0_TEMP1_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x051)
#define SPI_PAGE0_TEMP2_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x052)
#define SPI_PAGE0_TEMP2_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x053)
#define SPI_PAGE0_TEMP3_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x054)
#define SPI_PAGE0_TEMP3_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x055)
#define SPI_PAGE0_THERM_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x056)
#define SPI_PAGE0_THERM_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x057)
#define SPI_PAGE0_BARO_SUBSMP_ADDR            (SPI_PAGE0_BASE_ADDR + 0x058)
#define SPI_PAGE0_BARO_OVS_ADDR               (SPI_PAGE0_BASE_ADDR + 0x059)
#define SPI_PAGE0_EXTP_SUBSMP_ADDR            (SPI_PAGE0_BASE_ADDR + 0x05A)
#define SPI_PAGE0_EXTP_OVS_ADDR               (SPI_PAGE0_BASE_ADDR + 0x05B)
#define SPI_PAGE0_WD_SUBSMP_ADDR              (SPI_PAGE0_BASE_ADDR + 0x05C)
#define SPI_PAGE0_WD_OVS_ADDR                 (SPI_PAGE0_BASE_ADDR + 0x05D)
#define SPI_PAGE0_DIAG1_CFG_ADDR              (SPI_PAGE0_BASE_ADDR + 0x05E)
#define SPI_PAGE0_DIAG1_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x05F)
#define SPI_PAGE0_DIAG1_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x060)
#define SPI_PAGE0_DIAG1_C2V_CFG1_ADDR         (SPI_PAGE0_BASE_ADDR + 0x061)
#define SPI_PAGE0_DIAG1_C2V_CFG2_ADDR         (SPI_PAGE0_BASE_ADDR + 0x062)
#define SPI_PAGE0_DIAG1_C2V_CFG3_ADDR         (SPI_PAGE0_BASE_ADDR + 0x063)
#define SPI_PAGE0_DIAG2_CFG_ADDR              (SPI_PAGE0_BASE_ADDR + 0x064)
#define SPI_PAGE0_DIAG2_OVS_ADDR              (SPI_PAGE0_BASE_ADDR + 0x065)
#define SPI_PAGE0_DIAG2_SUBSMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x066)
#define SPI_PAGE0_DIAG2_C2V_CFG1_ADDR         (SPI_PAGE0_BASE_ADDR + 0x067)
#define SPI_PAGE0_DIAG2_C2V_CFG2_ADDR         (SPI_PAGE0_BASE_ADDR + 0x068)
#define SPI_PAGE0_DIAG2_C2V_CFG3_ADDR         (SPI_PAGE0_BASE_ADDR + 0x069)
#define SPI_PAGE0_HEATER_SUBSMP_ADDR          (SPI_PAGE0_BASE_ADDR + 0x06A)
#define SPI_PAGE0_HEATER_CFG_ADDR             (SPI_PAGE0_BASE_ADDR + 0x06B)
#define SPI_PAGE0_HTR_MAX_CUR_ADDR            (SPI_PAGE0_BASE_ADDR + 0x06C)
#define SPI_PAGE0_ODR_CONFIG_ADDR             (SPI_PAGE0_BASE_ADDR + 0x06F)
#define SPI_PAGE0_ASICST_INPUT_ADDR           (SPI_PAGE0_BASE_ADDR + 0x070)
#define SPI_PAGE0_COMP_ADDR                   (SPI_PAGE0_BASE_ADDR + 0x071)
#define SPI_PAGE0_MEAS_CFG_ADDR               (SPI_PAGE0_BASE_ADDR + 0x072)
#define SPI_PAGE0_INT_SHORT_DET_CFG_ADDR      (SPI_PAGE0_BASE_ADDR + 0x077)
#define SPI_PAGE0_HEATER_DTEMP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x079)
#define SPI_PAGE0_HEATER_NSTEP_ADDR           (SPI_PAGE0_BASE_ADDR + 0x07A)
#define SPI_PAGE0_HEATER_DTSTEP_PERIOD_ADDR   (SPI_PAGE0_BASE_ADDR + 0x07B)
#define SPI_PAGE0_HEATER_PATTERN_ADDR         (SPI_PAGE0_BASE_ADDR + 0x07C)
#define SPI_PAGE0_HEATER_ERROR_ADDR           (SPI_PAGE0_BASE_ADDR + 0x07D)
#define SPI_PAGE0_CMD_ADDR                    (SPI_PAGE0_BASE_ADDR + 0x07E)
#define SPI_PAGE0_EXT_MODE_ADDR               (SPI_PAGE0_BASE_ADDR + 0x07F)

//-----------------------------------------------------------------------------
// Address block: SPI_PAGE2
//-----------------------------------------------------------------------------
#define SPI_PAGE2_BASE_ADDR                   0x100
// #define SPI_PAGE2_LENGTH                      0x080
#define SPI_PAGE2_LAST_ADDR                   0x17F

#define SPI_PAGE2_MANU_ID_ADDR                (SPI_PAGE2_BASE_ADDR + 0x000)
#define SPI_PAGE2_CHIP_ID_ADDR                (SPI_PAGE2_BASE_ADDR + 0x001)
#define SPI_PAGE2_SI_ID_ADDR                  (SPI_PAGE2_BASE_ADDR + 0x002)
#define SPI_PAGE2_PKG_ID_ADDR                 (SPI_PAGE2_BASE_ADDR + 0x003)
#define SPI_PAGE2_TRIM_ID_ADDR                (SPI_PAGE2_BASE_ADDR + 0x004)

#define SPI_PAGE2_BDG_ADDR0                   (SPI_PAGE2_BASE_ADDR + 0x008)
#define SPI_PAGE2_BDG_ADDR1                   (SPI_PAGE2_BASE_ADDR + 0x009)
#define SPI_PAGE2_BDG_CMD                     (SPI_PAGE2_BASE_ADDR + 0x00A)

#define SPI_PAGE2_BDG_MASK0                   (SPI_PAGE2_BASE_ADDR + 0x00B)
#define SPI_PAGE2_BDG_MASK1                   (SPI_PAGE2_BASE_ADDR + 0x00C)
#define SPI_PAGE2_BDG_MASK2                   (SPI_PAGE2_BASE_ADDR + 0x00D)
#define SPI_PAGE2_BDG_MASK3                   (SPI_PAGE2_BASE_ADDR + 0x00E)
#define SPI_PAGE2_BDG_DATA0                   (SPI_PAGE2_BASE_ADDR + 0x00F)
#define SPI_PAGE2_BDG_DATA1                   (SPI_PAGE2_BASE_ADDR + 0x010)
#define SPI_PAGE2_BDG_DATA2                   (SPI_PAGE2_BASE_ADDR + 0x011)
#define SPI_PAGE2_BDG_DATA3                   (SPI_PAGE2_BASE_ADDR + 0x012)
#define SPI_PAGE2_BDG_STATUS                  (SPI_PAGE2_BASE_ADDR + 0x013)
#define SPI_PAGE2_TST_H_CONFIG                (SPI_PAGE2_BASE_ADDR + 0x014)

#define SPI_PAGE2_EXT_MODE                    (SPI_PAGE2_BASE_ADDR + 0x07F)

#define MANU_ID                               0xe4
#define CHIP_ID                               0x38

// Measurement Channel
#define TEMP1                                 0x0
#define TEMP2                                 0x1
#define TEMP3                                 0x2
#define THERM                                 0x3
#define BARO                                  0x4 
#define EXTP                                  0x5 
#define WD                                    0x6   
#define DIAG1                                 0x7
#define DIAG2                                 0x8 
#define HEATER                                0x9
#define FLAG_MEAS                             0xA

// INT 
#define EMU_INT_SOURCE_FIFO_FULL_EN           0x02
#define EMU_INT_SOURCE_FIFO_THS_EN            0x04
#define EMU_INT_SOURCE_DRDY_DATA_REG_EN       0x08
#define EMU_INT_SOURCE_MEAS_ODR_ERR_EN        0x20
#define EMU_INT_SOURCE_HTR_ERR_EN             0x40

#define EMU_INT_CONFIG_INT_POL_LOW            0x00
#define EMU_INT_CONFIG_INT_POL_HIGH           0x01
#define EMU_INT_CONFIG_INT_MODE_PULSED        0x00
#define EMU_INT_CONFIG_INT_MODE_LATCHED       0x04

#define EMU_INT_STATUS_FIFO_FULL              0x02
#define EMU_INT_STATUS_FIFO_THS               0x04
#define EMU_INT_STATUS_DRDY_DATA_REG          0x08
#define EMU_INT_STATUS_POR_STATUS             0x10
#define EMU_INT_STATUS_MEAS_ODR_ERR           0x20
#define EMU_INT_STATUS_HTR_ERR                0x40

// FIFO
#define EMU_FIFO_CONFIG_FIFO_MODE_BYPASS_0    0x00
#define EMU_FIFO_CONFIG_FIFO_MODE_STR         0x40
#define EMU_FIFO_CONFIG_FIFO_MODE_SOF         0x80
#define EMU_FIFO_CONFIG_FIFO_MODE_BYPASS_3    0xC0

#define EMU_FIFO_FRAME_CFG_LSB_TEMP1          0x01
#define EMU_FIFO_FRAME_CFG_LSB_TEMP2          0x02
#define EMU_FIFO_FRAME_CFG_LSB_TEMP3          0x04
#define EMU_FIFO_FRAME_CFG_LSB_THERM          0x08
#define EMU_FIFO_FRAME_CFG_LSB_BARO           0x10
#define EMU_FIFO_FRAME_CFG_LSB_EXTP           0x20
#define EMU_FIFO_FRAME_CFG_LSB_WD             0x40
#define EMU_FIFO_FRAME_CFG_LSB_DIAG1          0x80
#define EMU_FIFO_FRAME_CFG_MSB_DIAG2          0x01
#define EMU_FIFO_FRAME_CFG_MSB_HEATER         0x02
#define EMU_FIFO_FRAME_CFG_MSB_FLAG_MEAS      0x04

// HTR_ERROR
#define EMU_HEATER_ERROR_HTR_MAX_CUR			0x01
#define EMU_HEATER_ERROR_HTR_ODR_T 				0x02
#define EMU_HEATER_ERROR_HTR_ODR_H				0x03
#define EMU_HEATER_ERROR_HTR_FREE_RUN			0x04
#define EMU_HEATER_ERROR_HTR_DUR				0x05
#define EMU_HEATER_ERROR_HTR_PROD				0x06
#define EMU_HEATER_ERROR_HTR_LOCK_OUT			0x07
#define EMU_HEATER_ERROR_HTR_TMAX_ERR			0x08
#define EMU_HEATER_ERROR_HTR_VDD_MIN_ERR		0x09
#define EMU_HEATER_ERROR_HTR_UVLO_ERR 			0x0A
#define EMU_HEATER_ERROR_HTR_WATCHD_ERR			0x0B
#define EMU_HEATER_ERROR_HTR_SUBSMP_ERR			0x0C
#define EMU_HEATER_ERROR_HTR_DTOFS_ERR			0x0D

// MTP 
#define USR_TRIM_0_ADDR             0x0
#define USR_TRIM_1_ADDR             0x1
#define USR_TRIM_2_ADDR             0x2
#define USR_TRIM_3_ADDR             0x3
#define USR_TRIM_4_ADDR             0x4
#define USR_TRIM_5_ADDR             0x5

#define NVM_HIF_MODE_I2C_ONLY       0x0
#define NVM_HIF_MODE_SPI_12         0x1
#define NVM_HIF_MODE_SPI_03         0x2
#define NVM_HIF_MODE_AUTO           0x3
#define NVM_HIF_MODE_MASK           0x00000006

#define NVM_HIF_MODE_I2C_ONLY_CRC   0x61ed
#define NVM_HIF_MODE_SPI_12_CRC     0xcdf2
#define NVM_HIF_MODE_SPI_03_CRC     0x29F2
#define NVM_HIF_MODE_AUTO_CRC       0x85ed



#endif