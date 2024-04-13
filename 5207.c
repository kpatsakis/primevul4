static int acurite_txr_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    int browlen, valid = 0;
    uint8_t *bb;
    float tempf, wind_dir, wind_speed_kph, wind_speed_mph;
    uint8_t humidity, sequence_num, message_type;
    // uint8_t sensor_status;
    uint16_t sensor_id;
    int raincounter, battery_low;
    data_t *data;

    bitbuffer_invert(bitbuffer);

    for (uint16_t brow = 0; brow < bitbuffer->num_rows; ++brow) {
        browlen = (bitbuffer->bits_per_row[brow] + 7)/8;
        bb = bitbuffer->bb[brow];

        decoder_logf(decoder, 2, __func__, "row %u bits %u, bytes %d", brow, bitbuffer->bits_per_row[brow], browlen);

        if ((bitbuffer->bits_per_row[brow] < ACURITE_TXR_BITLEN ||
                bitbuffer->bits_per_row[brow] > ACURITE_5N1_BITLEN + 1)
                && bitbuffer->bits_per_row[brow] != ACURITE_6045_BITLEN
                && bitbuffer->bits_per_row[brow] != ACURITE_ATLAS_BITLEN
                && bitbuffer->bits_per_row[brow] != ACURITE_515_BITLEN) {
            if (bitbuffer->bits_per_row[brow] > 16)
                decoder_log(decoder, 2, __func__, "skipping wrong len");
            continue; // DECODE_ABORT_LENGTH
        }

        // There will be 1 extra false zero bit added by the demod.
        // this forces an extra zero byte to be added
        if (bb[browlen - 1] == 0)
            browlen--;

        // sum of first n-1 bytes modulo 256 should equal nth byte
        // also disregard a row of all zeros
        int sum = add_bytes(bb, browlen - 1);
        if (sum == 0 || (sum & 0xff) != bb[browlen - 1]) {
            decoder_log_bitrow(decoder, 1, __func__, bb, bitbuffer->bits_per_row[brow], "bad checksum");
            continue; // DECODE_FAIL_MIC
        }

        // acurite sensors with a common format appear to have a message type
        // in the lower 6 bits of the 3rd byte.
        // Format: PBMMMMMM
        // P = Parity
        // B = Battery Normal
        // M = Message type
        message_type = bb[2] & 0x3f;

        // Multiple AcuRite sensors use the same basic message format,
        // with a shared header and integrity scheme. These are differentiated
        // by the message_type.
        if (browlen == ACURITE_TXR_BITLEN / 8) {
            int decoded = 0;

            if (message_type == ACURITE_MSGTYPE_LEAK_DETECTOR) {
                decoded = acurite_leak_detector_decode(decoder, bb);
            } else if (message_type == ACURITE_MSGTYPE_TOWER_SENSOR) {
                decoded = acurite_tower_decode(decoder, bb);
            }

            // The decoder attempts for this size message will return a positive
            // value if they successfully decoded a message.
            if (decoded > 0) {
                valid++;
            }
        }

        // 515 sensor messages are 6 bytes.
        if (browlen == ACURITE_515_BITLEN / 8) {
            char const *channel_str = acurite_getChannelAndType(bb[0], message_type);

            // Sensor ID is the last 14 bits of byte 0 and 1
            // CCII IIII | IIII IIII
            // The sensor ID changes on each power-up of the sensor.
            sensor_id = ((bb[0] & 0x3f) << 8) | bb[1];

            // Sensor type (refrigerator, freezer) is determined by the message_type.
            if (message_type != ACURITE_MSGTYPE_515_REFRIGERATOR
                    && message_type != ACURITE_MSGTYPE_515_FREEZER) {
                decoder_logf(decoder, 2, __func__, "Acurite 515 sensor 0x%04X Ch %s, Unknown message type 0x%02x",
                        sensor_id, channel_str, message_type);
                continue; // DECODE_FAIL_MIC
            }

            // temperature encoding used by 515 sensors
            // 14 bits available after removing both parity bits.
            int temp_raw = ((bb[3] & 0x7F) << 7) | (bb[4] & 0x7F);
            tempf = (temp_raw - 1480) * 0.1f;
            // Battery status is the 7th bit 0x40. 1 = normal, 0 = low
            battery_low = (bb[2] & 0x40) == 0;

            /* clang-format off */
            data = data_make(
                    "model",                "",             DATA_STRING, "Acurite-515",
                    "id",                   "",             DATA_INT,    sensor_id,
                    "channel",              NULL,           DATA_STRING, channel_str,
                    "battery_ok",           "Battery",      DATA_INT,    !battery_low,
                    "temperature_F",        "Temperature",  DATA_FORMAT, "%.1f F", DATA_DOUBLE, tempf,
                    "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
                    NULL);
            /* clang-format on */

            decoder_output_data(decoder, data);
            valid++;
        }

        // The 5-n-1 weather sensor messages are 8 bytes.
        else if (message_type == ACURITE_MSGTYPE_5N1_WINDSPEED_WINDDIR_RAINFALL ||
                 message_type == ACURITE_MSGTYPE_5N1_WINDSPEED_TEMP_HUMIDITY ||
                 message_type == ACURITE_MSGTYPE_3N1_WINDSPEED_TEMP_HUMIDITY ||
                 message_type == ACURITE_MSGTYPE_RAINFALL) {
            decoder_log_bitrow(decoder, 1, __func__, bb, bitbuffer->bits_per_row[brow], "Acurite 5n1 raw msg");
            char const *channel_str = acurite_getChannel(bb[0]);

            // 5-n-1 sensor ID is the last 12 bits of byte 0 & 1
            // byte 0     | byte 1
            // CC RR IIII | IIII IIII
            sensor_id    = ((bb[0] & 0x0f) << 8) | bb[1];
            // The sensor sends the same data three times, each of these have
            // an indicator of which one of the three it is. This means the
            // checksum and first byte will be different for each one.
            // The bits 5,4 of byte 0 indicate which copy of the 65 bit data string
            //  00 = first copy
            //  01 = second copy
            //  10 = third copy
            //  1100 xxxx  = channel A 1st copy
            //  1101 xxxx  = channel A 2nd copy
            //  1110 xxxx  = channel A 3rd copy
            sequence_num = (bb[0] & 0x30) >> 4;
            battery_low = (bb[2] & 0x40) == 0;

            // Only for 5N1, range: 0 to 159 kph
            // raw number is cup rotations per 4 seconds
            // http://www.wxforum.net/index.php?topic=27244.0 (found from weewx driver)
            int speed_raw = ((bb[3] & 0x1F) << 3)| ((bb[4] & 0x70) >> 4);
            wind_speed_kph = 0;
            if (speed_raw > 0) {
                wind_speed_kph = speed_raw * 0.8278 + 1.0;
            }

            if (message_type == ACURITE_MSGTYPE_5N1_WINDSPEED_WINDDIR_RAINFALL) {
                // Wind speed, wind direction, and rain fall
                wind_dir = acurite_5n1_winddirections[bb[4] & 0x0f] * 22.5f;

                // range: 0 to 99.99 in, 0.01 inch increments, accumulated
                raincounter = ((bb[5] & 0x7f) << 7) | (bb[6] & 0x7F);

                /* clang-format off */
                data = data_make(
                        "model",        "",   DATA_STRING,    "Acurite-5n1",
                        "message_type", NULL,   DATA_INT,       message_type,
                        "id",           NULL, DATA_INT,       sensor_id,
                        "channel",      NULL,   DATA_STRING,    channel_str,
                        "sequence_num",  NULL,   DATA_INT,      sequence_num,
                        "battery_ok",       "Battery",      DATA_INT,    !battery_low,
                        "wind_avg_km_h",   "wind_speed",   DATA_FORMAT,    "%.1f km/h", DATA_DOUBLE,     wind_speed_kph,
                        "wind_dir_deg", NULL,   DATA_FORMAT,    "%.1f", DATA_DOUBLE,    wind_dir,
                        "rain_in",      "Rainfall Accumulation",   DATA_FORMAT, "%.2f in", DATA_DOUBLE, raincounter * 0.01f,
                        "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
                        NULL);
                /* clang-format on */

                decoder_output_data(decoder, data);
                valid++;
            }
            else if (message_type == ACURITE_MSGTYPE_5N1_WINDSPEED_TEMP_HUMIDITY) {
                // Wind speed, temperature and humidity

                // range -40 to 158 F
                int temp_raw = (bb[4] & 0x0F) << 7 | (bb[5] & 0x7F);
                tempf = (temp_raw - 400) * 0.1f;

                humidity = (bb[6] & 0x7f); // 1-99 %rH

                /* clang-format off */
                data = data_make(
                        "model",        "",   DATA_STRING,    "Acurite-5n1",
                        "message_type", NULL,   DATA_INT,       message_type,
                        "id",           NULL, DATA_INT,  sensor_id,
                        "channel",      NULL,   DATA_STRING,    channel_str,
                        "sequence_num",  NULL,   DATA_INT,      sequence_num,
                        "battery_ok",       "Battery",      DATA_INT,    !battery_low,
                        "wind_avg_km_h",   "wind_speed",   DATA_FORMAT,    "%.1f km/h", DATA_DOUBLE,     wind_speed_kph,
                        "temperature_F",     "temperature",    DATA_FORMAT,    "%.1f F", DATA_DOUBLE,    tempf,
                        "humidity",     NULL,    DATA_FORMAT,    "%u %%",   DATA_INT,   humidity,
                        "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
                        NULL);
                /* clang-format on */

                decoder_output_data(decoder, data);
                valid++;
            }
            else if (message_type == ACURITE_MSGTYPE_3N1_WINDSPEED_TEMP_HUMIDITY) {
                // Wind speed, temperature and humidity for 3-n-1
                sensor_id = ((bb[0] & 0x3f) << 8) | bb[1]; // 3-n-1 sensor ID is the bottom 14 bits of byte 0 & 1
                humidity = (bb[3] & 0x7f); // 1-99 %rH

                // note the 3n1 seems to have one more high bit than 5n1
                int temp_raw = (bb[4] & 0x1F) << 7 | (bb[5] & 0x7F);
                tempf        = (temp_raw - 1480) * 0.1f; // regression yields (rawtemp-1480)*0.1

                wind_speed_mph = bb[6] & 0x7f; // seems to be plain MPH

                /* clang-format off */
                data = data_make(
                        "model",        "",   DATA_STRING,    "Acurite-3n1",
                        "message_type", NULL,   DATA_INT,       message_type,
                        "id",    NULL,   DATA_FORMAT,    "0x%02X",   DATA_INT,       sensor_id,
                        "channel",      NULL,   DATA_STRING,    channel_str,
                        "sequence_num",  NULL,   DATA_INT,      sequence_num,
                        "battery_ok",       "Battery",      DATA_INT,    !battery_low,
                        "wind_avg_mi_h",   "wind_speed",   DATA_FORMAT,    "%.1f mi/h", DATA_DOUBLE,     wind_speed_mph,
                        "temperature_F",     "temperature",    DATA_FORMAT,    "%.1f F", DATA_DOUBLE,    tempf,
                        "humidity",     NULL,    DATA_FORMAT,    "%u %%",   DATA_INT,   humidity,
                        "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
                        NULL);
                /* clang-format on */

                decoder_output_data(decoder, data);
                valid++;
            }
            else if (message_type == ACURITE_MSGTYPE_RAINFALL) {
                // Rain Fall Gauge 899
                // The high 2 bits of byte zero are the channel (bits 7,6), 00 = A, 01 = B, 10 = C
                int channel = bb[0] >> 6;
                raincounter = ((bb[5] & 0x7f) << 7) | (bb[6] & 0x7f); // one tip is 0.01 inch, i.e. 0.254mm

                /* clang-format off */
                data = data_make(
                        "model",            "",                         DATA_STRING, "Acurite-Rain899",
                        "id",               "",                         DATA_INT,    sensor_id,
                        "channel",          "",                         DATA_INT,    channel,
                        "battery_ok",       "Battery",                  DATA_INT,    !battery_low,
                        "rain_mm",          "Rainfall Accumulation",    DATA_FORMAT, "%.2f mm", DATA_DOUBLE, raincounter * 0.254,
                        "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
                        NULL);
                /* clang-format on */

                decoder_output_data(decoder, data);
                valid++;
            }
            else {
                decoder_logf(decoder, 2, __func__, "Acurite 5n1 sensor 0x%04X Ch %s, Status %02X, Unknown message type 0x%02x",
                        sensor_id, channel_str, bb[3], message_type);
            }
        }

        else if (message_type == ACURITE_MSGTYPE_6045M) {
            // TODO: check parity and reject if invalid
            valid += acurite_6045_decode(decoder, bitbuffer, brow);
        }

        else if ((message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_TEMP_HUM ||
                  message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_RAIN ||
                  message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_UV_LUX ||
                  message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_TEMP_HUM_LTNG ||
                  message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_RAIN_LTNG ||
                  message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_UV_LUX_LTNG)) {
            valid += acurite_atlas_decode(decoder, bitbuffer, brow);
        }
    }

    return valid;
}