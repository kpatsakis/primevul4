static int acurite_atlas_decode(r_device *decoder, bitbuffer_t *bitbuffer, unsigned row)
{
    uint8_t humidity, sequence_num, message_type;
    char raw_str[31], *rawp;
    uint16_t sensor_id;
    int raincounter, battery_low;
    int exception = 0;
    float tempf, wind_dir, wind_speed_mph;
    data_t *data;

    int browlen = (bitbuffer->bits_per_row[row] + 7) / 8;
    uint8_t *bb = bitbuffer->bb[row];

    // {80} 82 f3 65 00 88 72 22 00 9f 95  {80} 86 f3 65 00 88 72 22 00 9f 99  {80} 8a f3 65 00 88 72 22 00 9f 9d
    // {80} 82 f3 66 00 05 e4 81 00 9f e4  {80} 86 f3 66 00 05 e4 81 00 9f e8  {80} 8a f3 66 00 05 e4 81 00 9f ec
    // {80} 82 f3 e7 00 00 00 96 00 9f 91  {80} 86 f3 e7 00 00 00 96 00 9f 95  {80} 8a f3 e7 00 00 00 96 00 9f 99
    // {80} 82 f3 66 00 05 60 81 00 9f 60  {80} 86 f3 66 00 05 60 81 00 9f 64  {80} 8a f3 66 00 05 60 81 00 9f 68
    // {80} 82 f3 65 00 88 71 24 00 9f 96  {80} 86 f3 65 00 88 71 24 00 9f 9a  {80} 8a f3 65 00 88 71 24 00 9f 9e
    // {80} 82 f3 65 00 88 71 a5 00 9f 17  {80} 86 f3 65 00 88 71 a5 00 9f 1b  {80} 8a f3 65 00 88 71 a5 00 9f 1f

    // decoder_log_bitrow(decoder, 0, __func__, bb, bitbuffer->bits_per_row[brow], "Acurite Atlas raw msg");
    message_type = bb[2] & 0x3f;
    sensor_id = ((bb[0] & 0x03) << 8) | bb[1];
    char const *channel_str = acurite_getChannel(bb[0]);

    // There are still a few unknown/unused bits in the message that
    // message that could possibly hold some data. Add the raw message hex to
    // to the structured data output to allow future analysis without
    // having to enable debug for long running rtl_433 processes.
    rawp = (char *)raw_str;
    for (int i=0; i < MIN(browlen, 15); i++) {
        sprintf(rawp,"%02x",bb[i]);
        rawp += 2;
    }
    *rawp = '\0';

    // The sensor sends the same data three times, each of these have
    // an indicator of which one of the three it is. This means the
    // checksum and first byte will be different for each one.
    // The bits 4,5 of byte 0 indicate which copy
    //  xxxx 00 xx = first copy
    //  xxxx 01 xx = second copy
    //  xxxx 10 xx = third copy
    sequence_num = (bb[0] & 0x0c) >> 2;
    // Battery status is the 7th bit 0x40. 1 = normal, 0 = low
    battery_low = (bb[2] & 0x40) == 0;

    // Wind speed is 8-bits raw MPH
    wind_speed_mph = ((bb[3] & 0x7F) << 1) | ((bb[4] & 0x40) >> 6);

    /* clang-format off */
    data = data_make(
            "model",                "",             DATA_STRING, "Acurite-Atlas",
            "id",                   NULL,           DATA_INT,    sensor_id,
            "channel",              NULL,           DATA_STRING, channel_str,
            "sequence_num",         NULL,           DATA_INT,    sequence_num,
            "battery_ok",           "Battery",      DATA_INT,    !battery_low,
            "message_type",         NULL,           DATA_INT,    message_type,
            "wind_avg_mi_h",        "Wind Speed",   DATA_FORMAT, "%.1f mi/h", DATA_DOUBLE, wind_speed_mph,
            NULL);
    /* clang-format on */

    if (message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_TEMP_HUM ||
            message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_TEMP_HUM_LTNG) {
        // Wind speed, temperature and humidity

        // range -40 to 160 F
        // TODO: are there really 13 bits? use 11 for now.
        int temp_raw = (bb[4] & 0x0F) << 7 | (bb[5] & 0x7F);
        tempf = (temp_raw - 400) * 0.1;

        humidity = (bb[6] & 0x7f); // 1-99 %rH

        /* clang-format off */
        data = data_append(data,
                "temperature_F",    "temperature",  DATA_FORMAT,    "%.1f F",       DATA_DOUBLE, tempf,
                "humidity",         NULL,           DATA_FORMAT,    "%u %%",        DATA_INT,    humidity,
                NULL);
        /* clang-format on */
    }

    if (message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_RAIN ||
            message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_RAIN_LTNG) {
        // Wind speed, wind direction, and rain fall
        wind_dir = ((bb[4] & 0x1f) << 5) | ((bb[5] & 0x7c) >> 2);

        // range: 0 to 5.11 in, 0.01 inch increments, accumulated
        // JRH: Confirmed 9 bits, counter rolls over after 5.11 inches
        raincounter = ((bb[5] & 0x03) << 7) | (bb[6] & 0x7F);

        /* clang-format off */
        data = data_append(data,
                "wind_dir_deg",     NULL,           DATA_FORMAT,    "%.1f",         DATA_DOUBLE, wind_dir,
                "rain_in",          "Rainfall Accumulation", DATA_FORMAT, "%.2f in", DATA_DOUBLE, raincounter * 0.01f,
                NULL);
        /* clang-format on */
    }

    if (message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_UV_LUX ||
            message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_UV_LUX_LTNG) {
        // Wind speed, UV Index, Light Intensity, Lightning?
        int uv  = (bb[4] & 0x0f);
        int lux = ((bb[5] & 0x7f) << 7) | (bb[6] & 0x7F);

        /* clang-format off */
        data = data_append(data,
                "uv",               NULL,           DATA_INT, uv,
                "lux",              NULL,           DATA_INT, lux * 10,
                NULL);
        /* clang-format on */
    }

    if ((message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_TEMP_HUM_LTNG ||
                message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_RAIN_LTNG ||
                message_type == ACURITE_MSGTYPE_ATLAS_WNDSPD_UV_LUX_LTNG)) {

        // @todo decode strike_distance to miles or KM.
        int strike_count    = ((bb[7] & 0x7f) << 2) | ((bb[8] & 0x60) >> 5);
        int strike_distance = bb[8] & 0x1f;

        /* clang-format off */
        data = data_append(data,
                "strike_count",         NULL,           DATA_INT, strike_count,
                "strike_distance",      NULL,           DATA_INT, strike_distance,
                NULL);
        /* clang-format on */
    }

    data = data_append(data,
            "exception",        "data_exception",   DATA_INT,    exception,    // @todo convert to bool
            "raw_msg",          "raw_message",      DATA_STRING, raw_str,
            NULL);

    decoder_output_data(decoder, data);

    return 1; // one valid message decoded
}