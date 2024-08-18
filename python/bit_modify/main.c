/*
 * main.c
 *
 *  Created on: Feb 9, 2015
 *      Author: asharma
 */
#include <stdbool.h>
#include <stdio.h>

typedef signed int int32_t;
#define NRT_COMPLETE 0

/*
 *****************************************************************************
 **
 ** @brief	Polling state machine for GPIO I2C Master transactions
 **
 ** Used by smb block read/write routines, the polling state machine has
 ** inline wait loops and does not yield at anytime.  The entry points
 ** are i2c_write_byte and i2c_read_byte
 **
 *****************************************************************************
 */
typedef enum {
    I2C_ERR_NONE = 0,
    I2C_ERR_TIMEOUT = 1,
    I2C_ERR_ARB_LOST = 2,
    I2C_ERR_NACK = 3
} i2c_error_t;

/*
 *****************************************************************************
 **
 ** @brief	Send the I2C start condition on the bus
 **
 ** @param[out]	bit_state - state of bit read on bus
 **
 ** @return     i2c_error_t
 **
 *****************************************************************************
 */
static i2c_error_t
i2c_read_bit(
  bool *                        bit_state)
{
    unsigned int                timeout_counter = 0;

    // Let the slave drive data
    //read_SDA();
    //I2C_delay();
    //while (read_SCL() == 0) { // Clock stretching
        if (timeout_counter++ > 2000) {
            return I2C_ERR_TIMEOUT;
        }
    //}
    // SCL is high, now data is valid
    *bit_state = 1;
    //I2C_delay();
    //clear_SCL();

    return I2C_ERR_NONE;
}

/*
 *****************************************************************************
 **
 ** @brief	write optional start condition, 8 data bits, optional stop
 **
 ** @param[in]	send_start - true to output start condition
 ** @param[in]	send_stop - true to output stop condition
 ** @param[in]	byte - data byte to output
 ** @param[out]	nack_from_slave - state of nack received from slave
 **
 ** @return     i2c_error_t
 **
 *****************************************************************************
 */
static i2c_error_t
i2c_write_byte(
  bool                          send_start,
  bool                          send_stop,
  unsigned char                 byte,
  bool *                        nack_from_slave)
{
    i2c_error_t                 status = I2C_ERR_NONE;

    if (send_start) {
        /*status = i2c_start_cond();
        if (status != I2C_ERR_NONE) {
            return status;
        }*/
    }

    /*for (i = 0; i < 8; i++) {
        status = i2c_write_bit((byte & 0x80) != 0);
        if (status != I2C_ERR_NONE) {
            return status;
        }
        byte <<= 1;
    }*/


    status = i2c_read_bit(nack_from_slave);
    if (status != I2C_ERR_NONE) {
        return status;
    }

    /*if (send_stop) {
        status = i2c_stop_cond();
    }*/

    return status;
}


/*
 *****************************************************************************
 **
 ** @brief	linux like smbus block write on i2c bus
 **
 ** @param[in]  pEnv            A pointer to an I2CMasterCtx_t context
 **
 ** @return     int32_t         A NRT-appropriate status
 **
 *****************************************************************************
 */
int32_t
i2c_smbus_write_block_data(
  void *                        pEnv)
{
    // start transfer with address write
    bool nack_from_slave;
    i2c_error_t                 i2c_stat;

    i2c_stat = i2c_write_byte(true, false, 0xff,
            &nack_from_slave);

    if (i2c_stat != I2C_ERR_NONE) {
        printf("%d\n",i2c_stat);
        return (NRT_COMPLETE);
    }
    if (nack_from_slave) {
    	printf("%d\n",I2C_ERR_NACK);
        return (NRT_COMPLETE);
    }

    return (NRT_COMPLETE);
}


int main(void){
	int *t = 0;
	i2c_smbus_write_block_data(t);
	return 0;
}
