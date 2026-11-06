#ifndef _GENERIC_H
#define _GENERIC_H

#include "stdio.h"
#include "horizon_addrmap.h"
#include "coines.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MAX(x, y) ((x) >= y ? x : y)

extern volatile uint8_t interrupt1_status;
extern void interrupt1_callback(uint32_t param1, uint32_t param2);

#define DEBUG true

#define SPI_RD_MASK         0x80
#define MTP_KEY             0x7B

#define ERROR_NONE 0
#define ERROR_REG_RW 1

#define MAX_SENSOR_NUM 7
#define MAX_DIAG_NUM 2
#define MAX_CHL_NUM (MAX_SENSOR_NUM+MAX_DIAG_NUM)

#define INT_PIN COINES_MINI_SHUTTLE_PIN_1_6

//=======================================================
// Hif Interface 
//=======================================================
extern volatile uint32_t DEF_USR_TRIM_0; 
extern volatile uint32_t DEF_USR_TRIM_1;
extern volatile uint32_t DEF_USR_TRIM_2;
extern volatile uint32_t DEF_USR_TRIM_3;
extern volatile uint32_t DEF_USR_TRIM_4;
extern volatile uint32_t DEF_USR_TRIM_5;

//=======================================================
// Hif Interface 
//=======================================================
enum spi_speed {
    SPI_SPEED_8MHZ   = COINES_SPI_SPEED_10_MHZ,
    SPI_SPEED_4MHZ   = COINES_SPI_SPEED_3_MHZ,
    SPI_SPEED_2MHZ   = COINES_SPI_SPEED_2_MHZ,
    SPI_SPEED_1MHZ   = COINES_SPI_SPEED_1_MHZ,
    SPI_SPEED_500KHZ = COINES_SPI_SPEED_500_KHZ,
    SPI_SPEED_250KHZ = COINES_SPI_SPEED_250_KHZ
};

typedef enum {
    SPI, // AUTO
    I2C, // AUTO
    SPI_03,
    SPI_12,
    I2C_ONLY
} HifType;

typedef struct {
    HifType type;
    union {
        struct {
            enum coines_spi_mode mode;                     
            enum spi_speed speed;
        } spi;

        struct {
            enum coines_i2c_mode mode;                     
            uint8_t dev_adddr; 
        } i2c;
    } cfg;
} HifConfig;

void config_hif_bus(const HifConfig *hif);
void hif_write(const HifConfig *hif, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);
void hif_read(const HifConfig *hif, uint8_t reg_addr, uint8_t *reg_data, uint16_t count);

extern HifConfig def_hif;

//=======================================================
// Measurement
//=======================================================

typedef struct {
    uint8_t subsmp;
    uint8_t start_pos;
    uint8_t suspend_run;
    uint8_t ovs_ofs;
    uint8_t ovs_mult;
    uint8_t cic_dec;
    uint8_t comp_en;
} ChannelConfig;

typedef struct {
    ChannelConfig   ch_cfg[MAX_SENSOR_NUM];
} SensorConfig;

typedef struct {
    uint8_t         meas_sel[MAX_DIAG_NUM];
    uint8_t         leakage_lna_en[MAX_DIAG_NUM];
    uint8_t         high_clk_f_en[MAX_DIAG_NUM];
    ChannelConfig   ch_cfg[MAX_DIAG_NUM];
    uint8_t         c2v_cfg1[MAX_DIAG_NUM];
    uint8_t         c2v_cfg2[MAX_DIAG_NUM];
    uint8_t         c2v_cfg3[MAX_DIAG_NUM];
} DiagConfig;

void config_sensors(const HifConfig *hif, const SensorConfig *sensor);
void config_diags(const HifConfig *hif, const DiagConfig *diag);
void config_odr(const HifConfig *hif, uint8_t odr_cfg, uint8_t odr_trig);
void cfg_data_ch_sel(const HifConfig *hif, uint8_t sens_no);

uint32_t get_odr_period_us(uint8_t odr_cfg);
void get_odr_freq(uint8_t odr_cfg, double *exp_odr);

//=======================================================
// FIFO
//=======================================================

typedef struct {
    uint8_t mode;
    uint8_t threshold;
    uint8_t frame_cfg_lsb;
    uint8_t frame_cfg_msb;
} FIFOConfig;

uint8_t config_fifo(const HifConfig *hif, const FIFOConfig *fifo);
uint8_t get_fifoDepth(const HifConfig *hif);
uint8_t get_fifoFrameSize(const HifConfig *hif);
uint8_t get_fifoCount(const HifConfig *hif);

//=======================================================
// Interrupt
//=======================================================

typedef struct {
    uint8_t pol;
    uint8_t mode;
    uint8_t source;
} InterruptConfig;

void config_interrupt(const HifConfig *hif, const InterruptConfig *interrupt);
uint8_t get_interruptStatus(const HifConfig *hif);

//=======================================================
// Miscellaneous
//=======================================================

void mtp_prog(const HifConfig *hif, uint8_t addr, uint8_t key, uint32_t mtp_wr_data);
void mtp_read(const HifConfig *hif, uint8_t addr, uint32_t *mtp_rd_data);
void store_mtp_init_data();
void load_mtp_init_data(const HifConfig *hif);

//=======================================================
// Miscellaneous
//=======================================================

void softReset(const HifConfig *hif);
uint8_t check_pin_value(enum coines_multi_io_pin pin_number, uint8_t exp_value);


#endif