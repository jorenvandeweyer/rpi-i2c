#include "gyroscope.h"

unsigned int refreshRate;
struct Vector_Float pos;
unsigned int last_time;

void setup_L3G4200D() {
    uprintf("Initialize L3G4200D\n\r");
    while(!L3G4200D_begin(L3G4200D_SCALE_2000DPS, L3G4200D_DATARATE_400HZ_50)) {
        uprintf("Could not find a valid L3G4200D sensor, check wiring!\n\r");
        delay_ms(200);
    }

    uprintf("Selected scale: ");
    switch (L3G4200D_getScale()) {
        case L3G4200D_SCALE_250DPS:
            uprintf("250 dps");
            break;
        case L3G4200D_SCALE_500DPS:
            uprintf("500 dps");
            break;
        case L3G4200D_SCALE_2000DPS:
            uprintf("2000 dps");
            break;
        default:
            uprintf("unknown");
            break;
    };

    uprintf("Output Data Rate: ");
    switch (L3G4200D_getOdrBw()) {
        case L3G4200D_DATARATE_800HZ_110:
            uprintf("800HZ, Cut-off 110");
            break;
        case L3G4200D_DATARATE_800HZ_50:
            uprintf("800HZ, Cut-off 50");
            break;
        case L3G4200D_DATARATE_800HZ_35:
            uprintf("800HZ, Cut-off 35");
            break;
        case L3G4200D_DATARATE_800HZ_30:
            uprintf("800HZ, Cut-off 30");
            break;
        case L3G4200D_DATARATE_400HZ_110:
            uprintf("400HZ, Cut-off 110");
            break;
        case L3G4200D_DATARATE_400HZ_50:
            uprintf("400HZ, Cut-off 50");
            break;
        case L3G4200D_DATARATE_400HZ_25:
            uprintf("400HZ, Cut-off 25");
            break;
        case L3G4200D_DATARATE_400HZ_20:
            uprintf("400HZ, Cut-off 20");
            break;
        case L3G4200D_DATARATE_200HZ_70:
            uprintf("200HZ, Cut-off 70");
            break;
        case L3G4200D_DATARATE_200HZ_50:
            uprintf("200HZ, Cut-off 50");
            break;
        case L3G4200D_DATARATE_200HZ_25:
            uprintf("200HZ, Cut-off 25");
            break;
        case L3G4200D_DATARATE_200HZ_12_5:
            uprintf("200HZ, Cut-off 12.5");
            break;
        case L3G4200D_DATARATE_100HZ_25:
            uprintf("100HZ, Cut-off 25");
            break;
        case L3G4200D_DATARATE_100HZ_12_5:
            uprintf("100HZ, Cut-off 12.5");
            break;
        default:
            uprintf("unknown");
            break;
    }
    uprintf("\n\r");
    
    L3G4200D_calibrate(50);
    L3G4200D_setThreshold(4);
}

void setup_gyroscope() {
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;

    setup_L3G4200D();
}

struct Vector_Float update_gyroscope() {
    unsigned int new_time = get_time_us();
    struct Vector_Float norm = L3G4200D_readNormalize();

    pos.x += ((float)(new_time-last_time)/1000000)*norm.x;
    pos.y += ((float)(new_time-last_time)/1000000)*norm.y;
    pos.z += ((float)(new_time-last_time)/1000000)*norm.z;
        
    last_time = new_time;

    uprintf("POS ");
    uprintf("x: %d° ", (int)pos.x);
    uprintf("y: %d° ", (int)pos.y);
    uprintf("z: %d° ", (int)pos.z);
    uprintf("\n\r");

    return pos;
}
