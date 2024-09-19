#include "cat.h"
#include "ui.h"

#include <algorithm>

#include "pico/stdlib.h"

void process_cat_control(rx_settings & settings_to_apply, rx_status & status, rx &receiver, uint32_t settings[])
{

    //if (!uart_is_readable()) return;
    bool settings_changed = false;
    char cmd[256];
    int32_t retval = stdio_get_until(cmd, sizeof(cmd), make_timeout_time_us(1000));
    if(retval == PICO_ERROR_TIMEOUT) return;
    const char mode_translation[] = "551243";

    if (strncmp(cmd, "FA", 2) == 0) {

        // Handle mode set/get commands
        if (cmd[2] == ';') {
            printf("FA%011lu;", settings[idx_frequency]);
        } else {
            uint32_t frequency_Hz;
            sscanf(cmd+2, "%lu", &frequency_Hz);
            if(frequency_Hz <= 30000000)
            {
              settings[idx_frequency]=frequency_Hz;
              settings_changed = true;
              //stdio_puts_raw("OK;");
            }
            else
            {
              stdio_puts_raw("?;");
            }
        }

    } else if (strncmp(cmd, "SM", 2) == 0) {

        // Handle mode set/get commands
        if (cmd[3] == ';') {
            receiver.access(false);
            float power_dBm = status.signal_strength_dBm;
            receiver.release();
            float power_scaled = 0xffff*((power_dBm - (-127))/114);
            power_scaled = std::min((float)0xffff, power_scaled);
            power_scaled = std::max((float)0, power_scaled);
            printf("SM%04X;", (uint16_t)power_scaled);
        } else {
            stdio_puts_raw("NG;");
        }

    } else if (strncmp(cmd, "MD", 2) == 0) {

        // Handle mode set/get commands
        if (cmd[2] == ';') {
            char mode_status = mode_translation[settings[idx_mode]];
            printf("MD%c;", mode_status);
        } else if (cmd[2] == '1') {
            settings_changed = true;
            settings[idx_mode] = MODE_LSB;
            //stdio_puts_raw("OK;");
        } else if (cmd[2] == '2') {
            settings_changed = true;
            settings[idx_mode] = MODE_USB;
            //stdio_puts_raw("OK;");
        } else if (cmd[2] == '3') {
            settings_changed = true;
            settings[idx_mode] = MODE_CW;
            //stdio_puts_raw("OK;");
        } else if (cmd[2] == '4') {
            settings_changed = true;
            settings[idx_mode] = MODE_FM;
            //stdio_puts_raw("OK;");
        } else if (cmd[2] == '5') {
            settings_changed = true;
            settings[idx_mode] = MODE_AM;
            //stdio_puts_raw("OK;");
        } else {
            //stdio_puts_raw("OK;");
        }

    } else if (strncmp(cmd, "IF", 2) == 0) {
        if (cmd[2] == ';') {
            printf("IF%011lu00000+0000000000%c0000000;", settings[idx_frequency], mode_translation[settings[idx_mode]]);
        }
    } else if (strncmp(cmd, "ID", 2) == 0) {
        if (cmd[2] == ';') {
            printf("ID020;");
        }
    } else if (strncmp(cmd, "AI", 2) == 0) {
        if (cmd[2] == ';') {
            printf("AI0;");
        }
    } else if (strncmp(cmd, "AG", 2) == 0) {
        if (cmd[2] == ';') {
            printf("AG0;");
        }
    } else if (strncmp(cmd, "XT", 2) == 0) {
        if (cmd[2] == ';') {
            printf("XT1;");
        }
    } else if (strncmp(cmd, "RT", 2) == 0) {
        if (cmd[2] == ';') {
            printf("RT1;");
        }
    } else if (strncmp(cmd, "RC", 2) == 0) {
        if (cmd[2] == ';') {
            printf("RC;");
        }
    } else if (strncmp(cmd, "FL", 2) == 0) {
        if (cmd[2] == ';') {
            printf("FL0;");
        }
    } else if (strncmp(cmd, "PS", 2) == 0) {
        if (cmd[2] == ';') {
            printf("PS1;");
        }
    } else if (strncmp(cmd, "VX", 2) == 0) {
        if (cmd[2] == ';') {
            printf("VX0;");
        }
    } else if (strncmp(cmd, "RS", 2) == 0) {
        if (cmd[2] == ';') {
            printf("RS0;");
        }
    } else if (strncmp(cmd, "FL", 2) == 0) {
        if (cmd[2] == ';') {
            printf("FL0;");
        }

    //fake TX for now
    } else if (strncmp(cmd, "TX", 2) == 0) {
        static uint8_t tx_status = 0;

        // Set or Get TX mode command (TX)
        if (cmd[2] == ';') {
            // Get current transmit status
            printf("TX%d;", tx_status);
        } else if (cmd[2] == '1') {
            // Switch to TX mode
            tx_status = 1;
            //stdio_puts_raw("OK;");
        } else if (cmd[2] == '0') {
            // Switch to RX mode
            tx_status = 0;
            //stdio_puts_raw("OK;");
        } else {
            // Invalid TX command format
            stdio_puts_raw("?;");
        }

    } else {
        // Unknown command
        stdio_puts_raw("?;");
    }

    //apply settings to receiver
    if(settings_changed)
    {
      receiver.access(true);
      settings_to_apply.tuned_frequency_Hz = settings[idx_frequency];
      settings_to_apply.agc_speed = settings[idx_agc_speed];
      settings_to_apply.enable_auto_notch = settings[idx_rx_features] >> flag_enable_auto_notch & 1;
      settings_to_apply.mode = settings[idx_mode];
      settings_to_apply.volume = settings[idx_volume];
      settings_to_apply.squelch = settings[idx_squelch];
      settings_to_apply.step_Hz = step_sizes[settings[idx_step]];
      settings_to_apply.cw_sidetone_Hz = settings[idx_cw_sidetone];
      settings_to_apply.bandwidth = settings[idx_bandwidth];
      settings_to_apply.gain_cal = settings[idx_gain_cal];
      receiver.release();
    }

}