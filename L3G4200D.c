#include "L3G4200D.h"
#include <stdio.h>
#include <math.h>
struct Vector_Float d;
struct Vector_Float t;
struct Vector_Short r;
struct Vector_Float n;

unsigned int useCalibrate;
unsigned int actualThreshold;
float dpsPerDigit;
float thresholdX;
float thresholdY;
float thresholdZ;

int L3G4200D_begin(l3g4200d_dps_t scale, l3g4200d_odrbw_t odrbw) {
    d.x = 0;
    d.y = 0;
    d.z = 0;
    useCalibrate = 0;

    t.x = 0;
    t.y = 0;
    t.z = 0;
    actualThreshold = 0;

    if (L3G4200D_fastRegister(L3G4200D_REG_WHO_AM_I) != 0xD3) {
        return 0;
    }

    unsigned int reg1 = 0x00;
    reg1 |= 0x0F;
    reg1 |= (odrbw << 4);

    L3G4200D_writeRegister(L3G4200D_REG_CTRL_REG1, reg1);
    L3G4200D_writeRegister(L3G4200D_REG_CTRL_REG2, 0x00);
    L3G4200D_writeRegister(L3G4200D_REG_CTRL_REG3, 0x08);
    L3G4200D_writeRegister(L3G4200D_REG_CTRL_REG4, scale << 4);

    switch (scale) {
        case L3G4200D_SCALE_250DPS:
            dpsPerDigit = .00875f; //114.28
            break;
        case L3G4200D_SCALE_500DPS:
            dpsPerDigit = .0175f; //57.14
            break;
        case L3G4200D_SCALE_2000DPS:
            dpsPerDigit = .07f; //14.28
            break;
        default:
            break;
    }

    L3G4200D_writeRegister(L3G4200D_REG_CTRL_REG5, 0x00);

    return 1;
}

l3g4200d_dps_t L3G4200D_getScale() {
    return (l3g4200d_dps_t)((L3G4200D_readRegister(L3G4200D_REG_CTRL_REG4) >> 4) & 0x03);
}

l3g4200d_odrbw_t L3G4200D_getOdrBw() {
    return (l3g4200d_odrbw_t)((L3G4200D_readRegister(L3G4200D_REG_CTRL_REG1) >> 4) & 0x0F);
}


// float sqroot(float square) {
//     float root=square/3;
//     int i;
//     if (square <= 0) return 0;
//     for (i=0; i<32; i++)
//         root = (root + square / root) / 2;
//     return root;
// }

void L3G4200D_calibrate(unsigned int samples) {
    useCalibrate = 1;

    float sumX = 0;
    float sumY = 0;
    float sumZ = 0;
    float sigmaX = 0;
    float sigmaY = 0;
    float sigmaZ = 0;

    for (unsigned int i = 3; i > 0; i--) {
        uprintf("calibrating in: %d seconds, please hold still\n\r", i);
        delay_ms(1000);
    }

    for (unsigned int i = 0; i < samples; i++) {
        L3G4200D_readRaw();

        sumX += r.x;
        sumY += r.y;
        sumZ += r.z;

        sigmaX += r.x * r.x;
        sigmaY += r.y * r.y;
        sigmaZ += r.z * r.z;

        delay_ms(20);
    }

    d.x = sumX / samples;
    d.y = sumY / samples;
    d.z = sumZ / samples;

    thresholdX = ((sigmaX / samples) - (d.x * d.x));
    thresholdY = ((sigmaY / samples) - (d.y * d.y));
    thresholdZ = ((sigmaZ / samples) - (d.z * d.z));

    uprintf("thresholdX %d\n\r", (int) thresholdX);

    if (actualThreshold > 0) {
        L3G4200D_setThreshold(actualThreshold);
    }
}



unsigned int L3G4200D_getThreshold() {
    return actualThreshold;
}

void L3G4200D_setThreshold(unsigned int multiple) {
    if (multiple > 0) {
        if (!useCalibrate) {
            uprintf("calibrating in set threshold\n\r");
            L3G4200D_calibrate(40);
        }

        t.x = thresholdX * multiple*multiple;
        t.y = thresholdY * multiple*multiple;
        t.z = thresholdZ * multiple*multiple;

        uprintf("tx: %d\n\r", (int)t.x);
    } else {
        t.x = 0;
        t.y = 0;
        t.z = 0;
    }

    actualThreshold = multiple;
}

void L3G4200D_writeRegister(unsigned int reg, unsigned int value) {
    unsigned int data[] = {reg, value};
    write_bytes(L3G4200D_ADDRESS, data, 2);
}

unsigned int L3G4200D_readRegister(unsigned int reg) {
    unsigned int value;
    unsigned int buffer[1];
    unsigned int data[] = {reg};

    write_bytes(L3G4200D_ADDRESS, data, 1);
    read_bytes(L3G4200D_ADDRESS, buffer, 1);
    value = buffer[0];
    return value;
}

unsigned int L3G4200D_fastRegister(unsigned int reg) {
    unsigned int buffer[1];
    unsigned int data[] = {reg};
    write_bytes(L3G4200D_ADDRESS, data, 1);
    read_bytes(L3G4200D_ADDRESS, buffer, 1);
    return buffer[0];
}

struct Vector_Short L3G4200D_readRaw() {
    unsigned int data[] = {(L3G4200D_REG_OUT_X_L | (1 << 7))};
    unsigned int buffer[6];

    write_bytes(L3G4200D_ADDRESS, data, 1);

    read_bytes(L3G4200D_ADDRESS, buffer, 6);

    r.x = (buffer[1] << 8 | buffer[0]) & 0xFFFF;
    r.y = (buffer[3] << 8 | buffer[2]) & 0xFFFF;
    r.z = (buffer[5] << 8 | buffer[4]) & 0xFFFF;

    return r;
}

struct Vector_Float L3G4200D_readNormalize() {
    L3G4200D_readRaw();

    if (useCalibrate) {
        n.x = (r.x - d.x) * dpsPerDigit;
        n.y = (r.y - d.y) * dpsPerDigit;
        n.z = (r.z - d.z) * dpsPerDigit;
    } else {
        n.x = r.x * dpsPerDigit;
        n.y = r.y * dpsPerDigit;
        n.z = r.z * dpsPerDigit;
    }

    if (actualThreshold > 0) {
        if (n.x*n.x < t.x*dpsPerDigit*dpsPerDigit) n.x = 0;
        if (n.y*n.y < t.y*dpsPerDigit*dpsPerDigit) n.y = 0;
        if (n.z*n.z < t.z*dpsPerDigit*dpsPerDigit) n.z = 0;
    }

    return n;
}

