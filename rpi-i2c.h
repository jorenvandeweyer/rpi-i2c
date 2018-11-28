#ifndef RPI_I2C_H
#define RPI_I2C_H

static volatile unsigned int* i2c_reg;

/**
 * Control register
*/

extern void i2c_init();
extern void i2c_enable();
extern void i2c_disable();
extern void enable_int_RX();
extern void disable_int_RX();
extern void enable_int_TX();
extern void disable_int_TX();
extern void enable_int_DONE();
extern void enable_int_DONE();
extern void start_transfer();
extern void clear_FIFO_data();
extern void set_read_transfer();
extern void set_write_transfer();

/**
 * Status register
*/

extern int is_CLKT();
extern void reset_CLKT();
extern int is_ERR();
extern void reset_ERR();
extern int is_RX_FIFO_full();
extern int is_TX_FIFO_empty();
extern int does_RX_contain_DATA();
extern int does_TX_accept_DATA();
extern int does_RX_need_reading();
extern int does_TX_need_writing();
extern int is_transfer_done();
extern int is_transfer_active();

/**
 * Data length register
 */

extern void set_data_length(int num);
extern int get_data_length();

/**
 * Slave address register
 */

extern void set_slave_address(int address);
extern int get_slave_address();

/**
 * Data FIFO register
 */

extern int get_FIFO_data();
extern void set_FIFO_data(int data);

/**
 * Clock divider register
 */

extern void set_clock_divider(int divisor);
extern int get_clock_divider();

/**
 * Data delay register
 */

extern void set_falling_edge_delay(int delay);
extern int get_falling_edge_delay();
extern void set_rising_edge_delay(int delay);
extern int get_rising_edge_delay();

/**
 * Clock stretch timeout register
 */

extern void set_clock_stretch_timeout(int timeout);
extern int get_clock_stretch_timeout();

/**
 *
 */

extern int read_bytes(int address, int no_bytes, char *bytes);

#endif
