static int acurite_leak_detector_decode(r_device* decoder, uint8_t* bb)
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
        decoder_logf(decoder, 1, __func__, "Acurite TXR sensor : bad channel Ch %s", channel_str);
        return DECODE_FAIL_SANITY;
    }

    // Tower sensor ID is the last 14 bits of byte 0 and 1
    // CCII IIII | IIII IIII
    int sensor_id = ((bb[0] & 0x3f) << 8) | bb[1];

    // Battery status is the 7th bit 0x40. 1 = normal, 0 = low
    int battery_low = (bb[2] & 0x40) == 0;

    // Leak indicator bit is the 5th bit of byte 3. 1 = wet, 0 = dry
    int is_wet = (bb[3] & 0x10) >> 4;

    data_t* data;
    /* clang-format off */
    data = data_make(
            "model",                "",             DATA_STRING, "Acurite-Leak",
            "id",                   "",             DATA_INT,    sensor_id,
            "channel",              NULL,           DATA_STRING, channel_str,
            "battery_ok",           "Battery",      DATA_INT,    !battery_low,
            "leak_detected",        "Leak",         DATA_INT,    is_wet,
            "mic",                  "Integrity",    DATA_STRING, "CHECKSUM",
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);

    return 1;
}