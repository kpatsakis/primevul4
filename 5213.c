static int acurite_tower_decode(r_device* decoder, uint8_t* bb)
{
    // checksum in the last byte has been validated in the calling function

    // Verify parity bits
    // Bytes 2, 3, 4, and 5 should all have a parity bit in their MSB
    int parity = parity_bytes(&bb[2], 4);
    if (parity) {
        decoder_log_bitrow(decoder, 1, __func__, bb, 7 * 8, "bad parity");
        return DECODE_FAIL_MIC;
    }

    // Channel is the first two bits of the 0th byte
    // but only 3 of the 4 possible values are valid
    char const* channel_str = acurite_getChannel(bb[0]);
    if (*channel_str == 'E') {
        decoder_logf(decoder, 1, __func__, "bad channel Ch %s", channel_str);
        return DECODE_FAIL_SANITY;
    }

    // Tower sensor ID is the last 14 bits of byte 0 and 1
    // CCII IIII | IIII IIII
    int sensor_id = ((bb[0] & 0x3f) << 8) | bb[1];

    // Battery status is the 7th bit 0x40. 1 = normal, 0 = low
    // pxxx xxxB
    int battery_low = (bb[2] & 0x40) == 0;

    // Humidity is stored in byte 3
    // The value is directly encoded as %rH
    // The possible values here are 0-128, but the manufacturer specifies that valid values
    // are only 1-99 %rH
    // pIII IIII
    int humidity = (bb[3] & 0x7f);
    if (humidity < 0 || humidity > 100) {
        decoder_logf(decoder, 1, __func__, "0x%04X Ch %s : Impossible humidity: %d %%rH",
                sensor_id, channel_str, humidity);
        return DECODE_FAIL_SANITY;
    }

    // temperature encoding used by "tower" sensors 592txr
    // 14 bits available after removing both parity bits.
    // 11 bits needed for specified range -40 C to 70 C (-40 F - 158 F)
    // Possible ranges are -100 C to 1538.4 C, but most of that range
    // is not possible on Earth.
    // pIII IIII pIII IIII
    int temp_raw = ((bb[4] & 0x7F) << 7) | (bb[5] & 0x7F);
    float tempc = temp_raw * 0.1 - 100;
    if (tempc < -40 || tempc > 70) {
        decoder_logf(decoder, 1, __func__, "0x%04X Ch %s : Impossible temperature: %0.2f C",
                sensor_id, channel_str, tempc);
        return DECODE_FAIL_SANITY;
    }

    data_t* data;
    /* clang-format off */
    data = data_make(
            "model",                "",             DATA_STRING, "Acurite-Tower",
            "id",                   "",             DATA_INT,    sensor_id,
            "channel",              NULL,           DATA_STRING, channel_str,
            "battery_ok",           "Battery",      DATA_INT,    !battery_low,
            "temperature_C",        "Temperature",  DATA_FORMAT, "%.1f C", DATA_DOUBLE, tempc,
            "humidity",             "Humidity",     DATA_FORMAT, "%u %%", DATA_INT,    humidity,
            "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);

    return 1;
}