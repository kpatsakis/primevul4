static int acurite_606_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    data_t *data;
    uint8_t *b;
    int row;
    int16_t temp_raw; // temperature as read from the data packet
    float temp_c;     // temperature in C
    int battery_ok;   // the battery status: 1 is good, 0 is low
    int sensor_id;    // the sensor ID - basically a random number that gets reset whenever the battery is removed

    row = bitbuffer_find_repeated_row(bitbuffer, 3, 32); // expected are 6 rows
    if (row < 0)
        return DECODE_ABORT_EARLY;

    if (bitbuffer->bits_per_row[row] > 33)
        return DECODE_ABORT_LENGTH;

    b = bitbuffer->bb[row];

    if (b[4] != 0)
        return DECODE_FAIL_SANITY;

    // reject all blank messages
    if (b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0)
        return DECODE_FAIL_SANITY;

    // calculate the checksum and only continue if we have a matching checksum
    uint8_t chk = lfsr_digest8(b, 3, 0x98, 0xf1);
    if (chk != b[3])
        return DECODE_FAIL_MIC;

    // Processing the temperature:
    // Upper 4 bits are stored in nibble 1, lower 8 bits are stored in nibble 2
    // upper 4 bits of nibble 1 are reserved for other usages (e.g. battery status)
    sensor_id  = b[0];
    battery_ok = (b[1] & 0x80) >> 7;
    temp_raw   = (int16_t)((b[1] << 12) | (b[2] << 4));
    temp_raw   = temp_raw >> 4;
    temp_c     = temp_raw * 0.1f;

    /* clang-format off */
    data = data_make(
            "model",            "",             DATA_STRING, "Acurite-606TX",
            "id",               "",             DATA_INT, sensor_id,
            "battery_ok",       "Battery",      DATA_INT,    battery_ok,
            "temperature_C",    "Temperature",  DATA_FORMAT, "%.1f C", DATA_DOUBLE, temp_c,
            "mic",              "Integrity",    DATA_STRING, "CHECKSUM",
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}