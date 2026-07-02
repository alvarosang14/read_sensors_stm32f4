#include "bno055_wrapper/bno055_wrapper.h"
#include "i2c.h"

#include "BNO055/bno055.h"

#define BNO055_I2C_TIMEOUT_MS 100

/*----------------------------------------------------------------------------*
 *  struct bno055_t parameters can be accessed by using BNO055
 *  BNO055_t having the following parameters
 *  Bus write function pointer: BNO055_WR_FUNC_PTR
 *  Bus read function pointer: BNO055_RD_FUNC_PTR
 *  Burst read function pointer: BNO055_BRD_FUNC_PTR
 *  Delay function pointer: delay_msec
 *  I2C address: dev_addr
 *  Chip id of the sensor: chip_id
 *---------------------------------------------------------------------------*/
static struct bno055_t bno055;
s32 comres = BNO055_ERROR;

u8 power_mode = BNO055_INIT_VALUE;

/*********read raw accel data***********/
s16 accel_datax = BNO055_INIT_VALUE;
s16 accel_datay = BNO055_INIT_VALUE;
s16 accel_dataz = BNO055_INIT_VALUE;
struct bno055_accel_t accel_xyz;

/*********read raw mag data***********/
s16 mag_datax = BNO055_INIT_VALUE;
s16 mag_datay = BNO055_INIT_VALUE;
s16 mag_dataz = BNO055_INIT_VALUE;
struct bno055_mag_t mag_xyz;

/***********read raw gyro data***********/
s16 gyro_datax = BNO055_INIT_VALUE;
s16 gyro_datay = BNO055_INIT_VALUE;
s16 gyro_dataz = BNO055_INIT_VALUE;
struct bno055_gyro_t gyro_xyz;

/*************read raw Euler data************/
s16 euler_data_h = BNO055_INIT_VALUE;
s16 euler_data_r = BNO055_INIT_VALUE;
s16 euler_data_p = BNO055_INIT_VALUE;
struct bno055_euler_t euler_hrp;

/************read raw quaternion data**************/
s16 quaternion_data_w = BNO055_INIT_VALUE;
s16 quaternion_data_x = BNO055_INIT_VALUE;
s16 quaternion_data_y = BNO055_INIT_VALUE;
s16 quaternion_data_z = BNO055_INIT_VALUE;
struct bno055_quaternion_t quaternion_wxyz;

/************read raw linear acceleration data***********/
s16 linear_accel_data_x = BNO055_INIT_VALUE;
s16 linear_accel_data_y = BNO055_INIT_VALUE;
s16 linear_accel_data_z = BNO055_INIT_VALUE;
struct bno055_linear_accel_t linear_acce_xyz;

/*****************read raw gravity sensor data****************/
s16 gravity_data_x = BNO055_INIT_VALUE;
s16 gravity_data_y = BNO055_INIT_VALUE;
s16 gravity_data_z = BNO055_INIT_VALUE;
struct bno055_gravity_t gravity_xyz;

/*************read accel converted data***************/
double d_accel_datax = BNO055_INIT_VALUE;
double d_accel_datay = BNO055_INIT_VALUE;
double d_accel_dataz = BNO055_INIT_VALUE;
struct bno055_accel_double_t d_accel_xyz;

/******************read mag converted data********************/
double d_mag_datax = BNO055_INIT_VALUE;
double d_mag_datay = BNO055_INIT_VALUE;
double d_mag_dataz = BNO055_INIT_VALUE;
struct bno055_mag_double_t d_mag_xyz;

/*****************read gyro converted data************************/
double d_gyro_datax = BNO055_INIT_VALUE;
double d_gyro_datay = BNO055_INIT_VALUE;
double d_gyro_dataz = BNO055_INIT_VALUE;
struct bno055_gyro_double_t d_gyro_xyz;

/*******************read euler converted data*******************/
double d_euler_data_h = BNO055_INIT_VALUE;
double d_euler_data_r = BNO055_INIT_VALUE;
double d_euler_data_p = BNO055_INIT_VALUE;
struct bno055_euler_double_t d_euler_hpr;

/*********read linear acceleration converted data**********/
double d_linear_accel_datax = BNO055_INIT_VALUE;
double d_linear_accel_datay = BNO055_INIT_VALUE;
double d_linear_accel_dataz = BNO055_INIT_VALUE;
struct bno055_linear_accel_double_t d_linear_accel_xyz;

/********************Gravity converted data**********************/
double d_gravity_data_x = BNO055_INIT_VALUE;
double d_gravity_data_y = BNO055_INIT_VALUE;
double d_gravity_data_z = BNO055_INIT_VALUE;
struct bno055_gravity_double_t d_gravity_xyz;

static s8 BNO055_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        &hi2c1, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT,
        reg_data, len, BNO055_I2C_TIMEOUT_MS);

    return (status == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static s8 BNO055_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 len) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        &hi2c1, (uint16_t)(dev_addr << 1), reg_addr, I2C_MEMADD_SIZE_8BIT,
        reg_data, len, BNO055_I2C_TIMEOUT_MS);

    return (status == HAL_OK) ? BNO055_SUCCESS : BNO055_ERROR;
}

static void BNO055_delay_msec(u32 msec) { HAL_Delay(msec); }

void bno055_init_a() {
    bno055.bus_write = BNO055_I2C_bus_write;
    bno055.bus_read = BNO055_I2C_bus_read;
    bno055.delay_msec = BNO055_delay_msec;
    bno055.dev_addr = BNO055_I2C_ADDR1;

    comres = bno055_init(&bno055);

    power_mode = BNO055_POWER_MODE_NORMAL;
    comres += bno055_set_power_mode(power_mode);

    // Inertial measurement unit.
    // Reads accel,gyro and fusion data
    comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_IMUPLUS);
}

void bno055_read() {
    /*  Raw accel X, Y and Z data can read from the register
     * page - page 0
     * register - 0x08 to 0x0D*/
    comres += bno055_read_accel_x(&accel_datax);
    comres += bno055_read_accel_y(&accel_datay);
    comres += bno055_read_accel_z(&accel_dataz);
    comres += bno055_read_accel_xyz(&accel_xyz);

    /*  Raw mag X, Y and Z data can read from the register
     * page - page 0
     * register - 0x0E to 0x13*/
    comres += bno055_read_mag_x(&mag_datax);
    comres += bno055_read_mag_y(&mag_datay);
    comres += bno055_read_mag_z(&mag_dataz);
    comres += bno055_read_mag_xyz(&mag_xyz);

    /*  Raw gyro X, Y and Z data can read from the register
     * page - page 0
     * register - 0x14 to 0x19*/
    comres += bno055_read_gyro_x(&gyro_datax);
    comres += bno055_read_gyro_y(&gyro_datay);
    comres += bno055_read_gyro_z(&gyro_dataz);
    comres += bno055_read_gyro_xyz(&gyro_xyz);
}

void bno055_convert() {
    /*  API used to read accel data output as double  - m/s2 and mg
     * float functions also available in the BNO055 API */
    comres += bno055_convert_double_accel_x_msq(&d_accel_datax);
    comres += bno055_convert_double_accel_x_mg(&d_accel_datax);
    comres += bno055_convert_double_accel_y_msq(&d_accel_datay);
    comres += bno055_convert_double_accel_y_mg(&d_accel_datay);
    comres += bno055_convert_double_accel_z_msq(&d_accel_dataz);
    comres += bno055_convert_double_accel_z_mg(&d_accel_dataz);
    comres += bno055_convert_double_accel_xyz_msq(&d_accel_xyz);
    comres += bno055_convert_double_accel_xyz_mg(&d_accel_xyz);

    /*  API used to read mag data output as double  - uT(micro Tesla)
     * float functions also available in the BNO055 API */
    comres += bno055_convert_double_mag_x_uT(&d_mag_datax);
    comres += bno055_convert_double_mag_y_uT(&d_mag_datay);
    comres += bno055_convert_double_mag_z_uT(&d_mag_dataz);
    comres += bno055_convert_double_mag_xyz_uT(&d_mag_xyz);

    /*  API used to read gyro data output as double  - dps and rps
     * float functions also available in the BNO055 API */
    comres += bno055_convert_double_gyro_x_dps(&d_gyro_datax);
    comres += bno055_convert_double_gyro_y_dps(&d_gyro_datay);
    comres += bno055_convert_double_gyro_z_dps(&d_gyro_dataz);
    comres += bno055_convert_double_gyro_x_rps(&d_gyro_datax);
    comres += bno055_convert_double_gyro_y_rps(&d_gyro_datay);
    comres += bno055_convert_double_gyro_z_rps(&d_gyro_dataz);
    comres += bno055_convert_double_gyro_xyz_dps(&d_gyro_xyz);
    comres += bno055_convert_double_gyro_xyz_rps(&d_gyro_xyz);
}

void bno055_stop() {
    power_mode = BNO055_POWER_MODE_SUSPEND;
    comres += bno055_set_power_mode(power_mode);
}