static int acurite_590tx_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    data_t *data;
    uint8_t *b;
    int row;
    int sensor_id;  // the sensor ID - basically a random number that gets reset whenever the battery is removed
    int battery_ok; // the battery status: 1 is good, 0 is low
    int channel;
    int humidity;
    int temp_raw; // temperature as read from the data packet
    float temp_c; // temperature in C

    row = bitbuffer_find_repeated_row(bitbuffer, 3, 25); // expected are min 3 rows
    if (row < 0)
        return DECODE_ABORT_EARLY;

    if (bitbuffer->bits_per_row[row] > 25)
        return DECODE_ABORT_LENGTH;

    b = bitbuffer->bb[row];

    if (b[4] != 0) // last byte should be zero
        return DECODE_FAIL_SANITY;

    // reject all blank messages
    if (b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0)
        return DECODE_FAIL_SANITY;

    // parity check: odd parity on bits [0 .. 10]
    // i.e. 8 bytes and another 2 bits.
    uint8_t parity = b[0]; // parity as byte
    parity = (parity >> 4) ^ (parity & 0xF); // fold to nibble
    parity = (parity >> 2) ^ (parity & 0x3); // fold to 2 bits
    parity ^= b[1] >> 6; // add remaining bits
    parity = (parity >> 1) ^ (parity & 0x1); // fold to 1 bit

    if (!parity) {
        decoder_log(decoder, 1, __func__, "parity check failed");
        return DECODE_FAIL_MIC;
    }

    // Processing the temperature:
    // Upper 4 bits are stored in nibble 1, lower 8 bits are stored in nibble 2
    // upper 4 bits of nibble 1 are reserved for other usages (e.g. battery status)
    sensor_id = b[0] & 0xFE; //first 6 bits and it changes each time it resets or change the battery
    battery_ok = (b[0] & 0x01); //1=ok, 0=low battery
    //next 2 bits are checksum
    //next two bits are identify ID (maybe channel ?)
    channel = (b[1] >> 4) & 0x03;

    temp_raw = (int16_t)(((b[1] & 0x0F) << 12) | (b[2] << 4));
    temp_raw = temp_raw >> 4;
    temp_c   = (temp_raw - 500) * 0.1f; // NOTE: there seems to be a 50 degree offset?

    if (temp_raw >= 0 && temp_raw <= 100) // NOTE: no other way to differentiate humidity from temperature?
        humidity = temp_raw;
    else
        humidity = -1;

    /* clang-format off */
     data = data_make(
            "model",            "",             DATA_STRING, "Acurite-590TX",
            "id",               "",             DATA_INT,    sensor_id,
            "battery_ok",       "Battery",      DATA_INT,    battery_ok,
            "channel",          "Channel",      DATA_INT,    channel,
            "humidity",         "Humidity",     DATA_COND,   humidity != -1,    DATA_INT,    humidity,
            "temperature_C",    "Temperature",  DATA_COND,   humidity == -1,    DATA_FORMAT, "%.1f C", DATA_DOUBLE, temp_c,
            "mic",              "Integrity",    DATA_STRING, "PARITY",
            NULL);
    /* clang-format on */

    decoder_output_data(decoder, data);
    return 1;
}