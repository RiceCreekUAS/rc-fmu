// module to query air data sensors

#include "src/BME280/BME280.h"
BME280 barometer;

#include "src/AMS5915/AMS5915.h"
AMS5915 ams_barometer;
AMS5915 ams_pitot;

#include "src/MS4525DO/MS4525DO.h"
MS4525DO ms45_pitot;

#include "src/MS5525DO/MS5525DO.h"
MS5525DO ms55_pitot;

int baro_status = -1;
float baro_press, baro_temp, baro_hum;

bool pitot_found = false;
bool ams_baro_found = false;

void airdata_defaults() {
    config.airdata.barometer = 0;
    config.airdata.pitot = 0;
    config.airdata.swift_baro_addr = 0;
    config.airdata.swift_pitot_addr = 0;
}

void airdata_setup() {
    if ( config.airdata.barometer == 0 ) {
        // BME280/SPI
        Serial.println("BME280 on SPI:26");
        barometer.configure(26);
        baro_status = barometer.begin();
    } else if (config.airdata.barometer == 1 ) {
        // BMP280/I2C
        Serial.println("BMP280 on I2C:0x76");
        barometer.configure(0x76, &Wire);
        baro_status = barometer.begin();
    } else if (config.airdata.barometer == 2 ) {
        // BFS Swift
        Serial.print("Swift barometer on I2C: 0x");
        Serial.println(config.airdata.swift_baro_addr, HEX);
        ams_barometer.configure(config.airdata.swift_baro_addr, &Wire1, AMS5915_1200_B);
        ams_barometer.begin();
        baro_status = 0;
    }
    if ( baro_status < 0 ) {
        Serial.println("Onboard barometer initialization unsuccessful");
        Serial.println("Check wiring or try cycling power");
        delay(1000);
    } else {
        Serial.println("Onboard barometer driver ready.");
    }

    if ( config.airdata.pitot == 0 ) {
        ms45_pitot.configure(0x28, &Wire);
        ms45_pitot.begin();
    } else if ( config.airdata.pitot == 1 ) {
        ms55_pitot.configure(0x76, &Wire);
        ms55_pitot.begin();
    } else if ( config.airdata.pitot == 2 ) {
        Serial.print("Swift pitot on I2C: 0x");
        Serial.println(config.airdata.swift_pitot_addr, HEX);
        ams_pitot.configure(config.airdata.swift_pitot_addr, &Wire1, AMS5915_0020_D);
        ams_pitot.begin();
    }
}

void airdata_update() {
    bool result;

    // read barometer (static pressure sensor)
    if ( baro_status >= 0 ) {
        if ( config.airdata.barometer == 0 || config.airdata.barometer == 1 ) {
            barometer.getData(&baro_press, &baro_temp, &baro_hum);
        } else if ( config.airdata.barometer == 2 ) {
            if ( ams_barometer.getData(&baro_press, &baro_temp) ) {
                ams_baro_found = true;
            } else {
                if ( ams_baro_found ) {
                    Serial.println("Error while reading sPress sensor.");
                    airdata_error_count++;
                }
            }
        }
    }

    if ( config.airdata.pitot == 0 ) {
        result = ms45_pitot.getData(&airdata_diffPress_pa, &airdata_temp_C);
    } else if ( config.airdata.pitot == 1 ) {
        result = ms55_pitot.getData(&airdata_diffPress_pa, &airdata_temp_C);
    } else if ( config.airdata.pitot == 2 ) {
        result = ams_pitot.getData(&airdata_diffPress_pa, &airdata_temp_C);
    } else {
        result = false;
    }
    if ( !result ) {
        if ( pitot_found ) {
            Serial.println("Error while reading pitot sensor.");
            airdata_error_count++;
        }
    } else {
        pitot_found = true;
    }
}
