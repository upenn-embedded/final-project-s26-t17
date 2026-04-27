//#include "lsm6ds.h"
//#include "spi.h"
//#include <stdint.h>
//
//#define LSM6DS_READ_MASK       0x80
//#define LSM6DS_AUTO_INC_MASK   0x40
//
///* LSM6DSO register map */
//#define LSM6DS_WHO_AM_I        0x0F
//#define LSM6DS_CTRL1_XL        0x10
//#define LSM6DS_CTRL2_G         0x11
//#define LSM6DS_CTRL3_C         0x12
//#define LSM6DS_STATUS_REG      0x1E
//#define LSM6DS_OUTX_L_A        0x28
//
//#define LSM6DS_WHO_AM_I_VAL    0x6C
//#define LSM6DS_STATUS_REG   0x1E
//
//
//
//
//static void lsm6ds0_write_reg(uint8_t reg, uint8_t value)
//{
//    uint8_t tx[2];
//
//    tx[0] = reg & 0x7F;   /* bit7 = 0 => write */
//    tx[1] = value;
//
//    cs_enable();
//    spi1_transmit(tx, 2);
//    cs_disable();
//}
//
//static uint8_t lsm6ds0_read_reg(uint8_t reg)
//{
//    uint8_t cmd = reg | LSM6DS_READ_MASK;
//    uint8_t value = 0;
//
//    cs_enable();
//    spi1_transmit(&cmd, 1);
//    spi1_receive(&value, 1);
//    cs_disable();
//
//    return value;
//}
//
//static void lsm6ds0_read_multi(uint8_t start_reg, uint8_t *data, uint32_t len)
//{
//    uint8_t cmd = start_reg | LSM6DS_READ_MASK | LSM6DS_AUTO_INC_MASK;
//
//    cs_enable();
//    spi1_transmit(&cmd, 1);
//    spi1_receive(data, len);
//    cs_disable();
//}
//
//uint8_t lsm6ds0_who_am_i(void)
//{
//    return lsm6ds0_read_reg(LSM6DS_WHO_AM_I);
//}
//
//void lsm6ds0_init(void)
//{
//    /* Gyro off */
//    lsm6ds0_write_reg(LSM6DS_CTRL2_G, 0x00);
//
//    /* CTRL1_XL:
//       ODR_XL = 0100 => 104 Hz
//       FS_XL  = 00   => +/-2g
//       LPF2_XL_EN = 0
//       last bit must be 0
//       => 0x40
//    */
//    lsm6ds0_write_reg(LSM6DS_CTRL1_XL, 0x40);
//
//    /* CTRL3_C:
//       BDU = 1
//       IF_INC = 1
//       => 0x44
//    */
//    lsm6ds0_write_reg(LSM6DS_CTRL3_C, 0x44);
//}
//
////void lsm6ds0_read_accel_raw(lsm6ds0_accel_raw_t *accel)
////{
////    uint8_t buf[6];
////
////    lsm6ds0_read_multi(LSM6DS_OUTX_L_A, buf, 6);
////
////    accel->x = (int16_t)((buf[1] << 8) | buf[0]);
////    accel->y = (int16_t)((buf[3] << 8) | buf[2]);
////    accel->z = (int16_t)((buf[5] << 8) | buf[4]);
////}
//
//
//void lsm6ds0_read_accel_raw(lsm6ds0_accel_raw_t *accel)
//{
//    uint8_t xl, xh, yl, yh, zl, zh;
//
//    xl = lsm6ds0_read_reg(0x28);
//    xh = lsm6ds0_read_reg(0x29);
//    yl = lsm6ds0_read_reg(0x2A);
//    yh = lsm6ds0_read_reg(0x2B);
//    zl = lsm6ds0_read_reg(0x2C);
//    zh = lsm6ds0_read_reg(0x2D);
//
//    accel->x = (int16_t)((xh << 8) | xl);
//    accel->y = (int16_t)((yh << 8) | yl);
//    accel->z = (int16_t)((zh << 8) | zl);
//}
//
//
//int lsm6ds0_test(lsm6ds0_accel_raw_t *accel)
//{
//    uint8_t who = lsm6ds0_who_am_i();
//
//    if (who != LSM6DS_WHO_AM_I_VAL)
//    {
//        return -1;
//    }
//
//    lsm6ds0_init();
//    lsm6ds0_read_accel_raw(accel);
//
//    return 0;
//}
//
//
//uint8_t lsm6ds0_read_status(void)
//{
//    return lsm6ds0_read_reg(LSM6DS_STATUS_REG);
//}
//
//uint8_t lsm6ds0_read_ctrl1_xl(void)
//{
//    return lsm6ds0_read_reg(LSM6DS_CTRL1_XL);
//}
//
//uint8_t lsm6ds0_read_ctrl3_c(void)
//{
//    return lsm6ds0_read_reg(LSM6DS_CTRL3_C);
//}
//
//
//uint8_t lsm6ds_read_status(void)
//{
//    return lsm6ds0_read_reg(LSM6DS_STATUS_REG);
//}
//
//void lsm6ds_wait_for_xlda(void)
//{
//    while (!(lsm6ds_read_status() & 0x01))
//    {
//    }
//}
//
//
