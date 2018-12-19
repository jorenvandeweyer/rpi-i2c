#include "L3G4200D.h"
#include <stdio.h>
#include <math.h>
struct Vector d;
struct Vector t;
struct Vector r;
struct Vector n;

unsigned int useCalibrate;
unsigned int actualThreshold;
int dpsPerDigit;
int thresholdX;
int thresholdY;
int thresholdZ;

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
            dpsPerDigit = .00875f;
            break;
        case L3G4200D_SCALE_500DPS:
            dpsPerDigit = .0175f;
            break;
        case L3G4200D_SCALE_2000DPS:
            dpsPerDigit = .07f;
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

    int sumX = 0;
    int sumY = 0;
    int sumZ = 0;
    int sigmaX = 0;
    int sigmaY = 0;
    int sigmaZ = 0;

    for (unsigned int i = 0; i < samples; i++) {
        L3G4200D_readRaw();
        sumX += r.x;
        sumY += r.y;
        sumZ += r.z;

        sigmaX += r.x * r.x;
        sigmaY += r.y * r.y;
        sigmaZ += r.z * r.z;

        volatile unsigned int tim;
        for (tim = 0; tim<10000;tim++);
    }

    d.x = sumX / samples;
    d.y = sumY / samples;
    d.z = sumZ / samples;

    thresholdX = ((sigmaX / samples) - (d.x * d.x));
    thresholdX = ((sigmaX / samples) - (d.x * d.x));
    thresholdX = ((sigmaX / samples) - (d.x * d.x));

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
            L3G4200D_calibrate(100);
        }

        t.x = thresholdX * multiple;
        t.y = thresholdY * multiple;
        t.z = thresholdZ * multiple;
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

struct Vector L3G4200D_readRaw() {
    unsigned int data[] = {(L3G4200D_REG_OUT_X_L | (1 << 7))};
    unsigned int buffer[6];

    write_bytes(L3G4200D_ADDRESS, data, 1);

    read_bytes(L3G4200D_ADDRESS, buffer, 6);

    r.x = buffer[1] << 8 | buffer[0];
    r.y = buffer[3] << 8 | buffer[2];
    r.z = buffer[5] << 8 | buffer[4];

    return r;
}

struct Vector L3G4200D_readNormalize() {
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
        if (abs(n.x) < t.x) n.x = 0;
        if (abs(n.y) < t.y) n.y = 0;
        if (abs(n.z) < t.z) n.z = 0;
    }

    return n;
}

