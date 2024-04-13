static int acurite_6045_decode(r_device *decoder, bitbuffer_t *bitbuffer, unsigned row)
{
    float tempf;
    uint8_t humidity;
    // uint8_t message_type, l_status;
    char raw_str[31], *rawp;
    uint16_t sensor_id;
    uint8_t strike_count, strike_distance;
    int battery_low, active, rfi_detect;
    int exception = 0;
    data_t *data;

    int browlen = (bitbuffer->bits_per_row[row] + 7) / 8;
    uint8_t *bb = bitbuffer->bb[row];

    char const *channel_str = acurite_getChannel(bb[0]); // same as TXR

    // Tower sensor ID is the last 14 bits of byte 0 and 1
    // CCII IIII | IIII IIII
    sensor_id = ((bb[0] & 0x3f) << 8) | bb[1]; // same as TXR
    battery_low = (bb[2] & 0x40) == 0;
    humidity = (bb[3] & 0x7f); // 1-99 %rH, same as TXR
    active = (bb[4] & 0x40) == 0x40;    // Sensor is actively listening for strikes
    //message_type = bb[2] & 0x3f;

    // 12 bits of temperature after removing parity and status bits.
    // Message native format appears to be in 1/10 of a degree Fahrenheit
    // Device Specification: -40 to 158 F  / -40 to 70 C
    // Available range given 12 bits with +1480 offset: -140.0 F to +261.5 F
    int temp_raw = ((bb[4] & 0x1F) << 7) | (bb[5] & 0x7F);
    tempf = (temp_raw - 1480) * 0.1f;

    // Strike count is 8 bits, LSB in following byte
    strike_count = ((bb[6] & 0x7f) << 1) | ((bb[7] & 0x40) >> 6);
    strike_distance = bb[7] & 0x1f;
    rfi_detect = (bb[7] & 0x20) == 0x20;
    //l_status = (bb[7] & 0x60) >> 5;

    /*
     * 2018-04-21 rct - There are still a number of unknown bits in the
     * message that need to be figured out. Add the raw message hex to
     * to the structured data output to allow future analysis without
     * having to enable debug for long running rtl_433 processes.
     */
    rawp = (char *)raw_str;
    for (int i=0; i < MIN(browlen, 15); i++) {
        sprintf(rawp,"%02x",bb[i]);
        rawp += 2;
    }
    *rawp = '\0';

    // Flag whether this message might need further analysis
    if (((bb[4] & 0x20) != 0) ||  // unknown status bits, always off
        (humidity > 100) ||
        (tempf > 158) ||
        (tempf < -40)) {
        exception++;
    }

    /* clang-format off */
    data = data_make(
            "model",            "",                 DATA_STRING, "Acurite-6045M",
            "id",               NULL,               DATA_INT,    sensor_id,
            "channel",          NULL,               DATA_STRING, channel_str,
            "battery_ok",       "Battery",          DATA_INT,    !battery_low,
            "temperature_F",    "temperature",      DATA_FORMAT, "%.1f F",     DATA_DOUBLE,     tempf,
            "humidity",         "humidity",         DATA_FORMAT, "%u %%", DATA_INT,    humidity,
            "strike_count",     "strike_count",     DATA_INT,    strike_count,
            "storm_dist",       "storm_distance",   DATA_INT,    strike_distance,
            "active",           "active_mode",      DATA_INT,    active,    // @todo convert to bool
            "rfi",              "rfi_detect",       DATA_INT,    rfi_detect,     // @todo convert to bool
            "exception",        "data_exception",   DATA_INT,    exception,    // @todo convert to bool
            "raw_msg",          "raw_message",      DATA_STRING, raw_str,
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}