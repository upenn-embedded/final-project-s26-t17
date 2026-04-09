/*
 * lsm6ds.h
 *
 *  Created on: Apr 7, 2026
 *      Author: david
 */

#ifndef LSM6DS_H_
#define LSM6DS_H_


#include <stdint.h>

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} lsm6ds0_accel_raw_t;

// Basic device functions
uint8_t lsm6ds0_who_am_i(void);
void lsm6ds0_init(void);

// Accelerometer read
void lsm6ds0_read_accel_raw(lsm6ds0_accel_raw_t *accel);

// Simple test helper
//   Returns 0 on success
//   Returns -1 if WHO_AM_I does not match expected value

int lsm6ds0_test(lsm6ds0_accel_raw_t *accel);



uint8_t lsm6ds0_read_status(void);

uint8_t lsm6ds0_read_ctrl1_xl(void);

uint8_t lsm6ds0_read_ctrl3_c(void);


uint8_t lsm6ds_read_status(void);

void lsm6ds_wait_for_xlda(void);

#endif /* LSM6DS_H_ */
