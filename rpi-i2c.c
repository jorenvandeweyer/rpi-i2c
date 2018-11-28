#include "rpi-i2c.h"
#include "defines.h"

/**
 * Control register
*/

void i2c_init() {
    i2c_reg = (unsigned int*) I2C_BASE;
}

void i2c_enable() {
    i2c_reg[I2C_CONTROL] |= (1 << 15);
}

void i2c_disable() {
    i2c_reg[I2C_CONTROL] &= ~(1 << 15);
}

void enable_int_RX() {
    i2c_reg[I2C_CONTROL] |= (1 << 10);
}

void disable_int_RX() {
    i2c_reg[I2C_CONTROL] &= ~(1 << 10);
}

void enable_int_TX() {
    i2c_reg[I2C_CONTROL] |= (1 << 9);
}

void disable_int_TX() {
    i2c_reg[I2C_CONTROL] &= ~(1 << 9);
}

void enable_int_DONE() {
    i2c_reg[I2C_CONTROL] |= (1 << 8);
}

void start_transfer() {
    i2c_enable();
    i2c_reg[I2C_CONTROL] |= (1 << 7);
}

void clear_FIFO_data() {
    i2c_reg[I2C_CONTROL] |= (1 << 4);
}

void set_read_transfer() {
    i2c_reg[I2C_CONTROL] |= 1;
}

void set_write_transfer() {
    i2c_reg[I2C_CONTROL] &= ~(1 << 0);
}

/**
 * Status register
*/

int is_CLKT() {
    return (i2c_reg[I2C_STATUS] >> 9) & 1;
}

void reset_CLKT() {
    i2c_reg[I2C_STATUS] |= (1 << 9);
}

int is_ERR() {
    return (i2c_reg[I2C_STATUS] >> 8) & 1;
}

void reset_ERR() {
    i2c_reg[I2C_STATUS] |= (1 << 8);
}

int is_RX_FIFO_full() {
    return (i2c_reg[I2C_STATUS] >> 7) & 1;
}

int is_TX_FIFO_empty() {
    return (i2c_reg[I2C_STATUS] >> 6) & 1;
}

int does_RX_contain_DATA() {
    return (i2c_reg[I2C_STATUS] >> 5) & 1;
}

int does_TX_accept_DATA() {
    return (i2c_reg[I2C_STATUS] >> 4) & 1;
}

int does_RX_need_reading() {
    return (i2c_reg[I2C_STATUS] >> 3) & 1;
}

int does_TX_need_writing() {
    return (i2c_reg[I2C_STATUS] >> 2) & 1;
}

int is_transfer_done() {
    return (i2c_reg[I2C_STATUS] >> 1) & 1;
}

void reset_transfer_done() {
    i2c_reg[I2C_STATUS] |= (1 >> 1);
}

int is_transfer_active() {
    return (i2c_reg[I2C_STATUS] >> 0) & 1;
}

void reset_status_register() {
    reset_CLKT();
    reset_ERR();
    reset_transfer_done();
}

/**
 * Data length register
 */

void set_data_length(int num) {
    i2c_reg[I2C_DLEN] = num;
}

int get_data_length() {
    return (i2c_reg[I2C_DLEN] << 16);
}

/**
 * Slave address register
 */

void set_slave_address(int address) {
    i2c_reg[I2C_SLAVE_ADDRESS] ^= address; 
}

int get_slave_address() {
    return (i2c_reg[I2C_SLAVE_ADDRESS] << 25); 
}

/**
 * Data FIFO register
 */

int get_FIFO_data() {
    return i2c_reg[DATA_FIFO] << 24;
}

void set_FIFO_data(int data) {
    i2c_reg[DATA_FIFO] = data;
}

/**
 * Clock divider register
 */

void set_clock_divider(int divisor) {
    i2c_reg[CLOCK_DIVIDER] ^= divisor;
}

int get_clock_divider() {
    return (i2c_reg[CLOCK_DIVIDER] << 16);
}

/**
 * Data delay register
 */

void set_falling_edge_delay(int delay) {
    i2c_reg[DATA_DELAY] ^= (delay << 16);
}

int get_falling_edge_delay() {
    return (i2c_reg[DATA_DELAY] >> 16);
}

void set_rising_edge_delay(int delay) {
    i2c_reg[DATA_DELAY] ^= delay;
}

int get_rising_edge_delay() {
    return (i2c_reg[DATA_DELAY] << 16);
}

/**
 * Clock stretch timeout register
 */

void set_clock_stretch_timeout(int timeout) {
    i2c_reg[CLOCK_STRETCH_TIMEOUT] ^= timeout;
}

int get_clock_stretch_timeout() {
    return (i2c_reg[CLOCK_STRETCH_TIMEOUT] << 16);
}

/**
 *
 */

int read_bytes(int address, int no_bytes, char *bytes) {
    set_slave_address(address);
    set_data_length(no_bytes);
    set_read_transfer();
    //enable_int_DONE();
    start_transfer();

    int i = 0;

    while (!is_transfer_done()) {
        while (does_RX_contain_DATA()) {
            bytes[i] = get_FIFO_data();
            i++;
        }
    } 
}

int read_byte(int address, char *byte) {
    reset_status_register();
    clear_FIFO_data();

    set_slave_address(address);
    set_data_length(1);
    set_FIFO_data(0xEE);
    set_read_transfer();
    
    start_transfer();
    while(!is_transfer_active());
    *byte = get_FIFO_data();
}

void write_bytes(int address, int *byte, unsigned int no_bytes) {
    reset_status_register();
    clear_FIFO_data();

    set_data_length(no_bytes+1);
    for (int i = 0; i<no_bytes; i++) {
        set_FIFO_data(byte[i]);
    }
    set_slave_address(address);
    set_write_transfer();
    start_transfer();   
}
