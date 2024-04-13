static int acurite_th_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    uint8_t *bb = NULL;
    int cksum, battery_low, valid = 0;
    float tempc;
    uint8_t humidity, id, status;
    data_t *data;
    int result = 0;

    for (uint16_t brow = 0; brow < bitbuffer->num_rows; ++brow) {
        if (bitbuffer->bits_per_row[brow] != 40) {
            result = DECODE_ABORT_LENGTH;
            continue; // DECODE_ABORT_LENGTH
        }

        bb = bitbuffer->bb[brow];

        cksum = (bb[0] + bb[1] + bb[2] + bb[3]);

        if (cksum == 0 || ((cksum & 0xff) != bb[4])) {
            result = DECODE_FAIL_MIC;
            continue; // DECODE_FAIL_MIC
        }

        // Temperature in Celsius is encoded as a 12 bit integer value
        // multiplied by 10 using the 4th - 6th nybbles (bytes 1 & 2)
        // negative values are recovered by sign extend from int16_t.
        int temp_raw = (int16_t)(((bb[1] & 0x0f) << 12) | (bb[2] << 4));
        tempc        = (temp_raw >> 4) * 0.1f;
        id           = bb[0];
        status       = (bb[1] & 0xf0) >> 4;
        battery_low  = status & 0x8;
        humidity     = bb[3];

        /* clang-format off */
        data = data_make(
                "model",            "",             DATA_STRING, "Acurite-609TXC",
                "id",               "",             DATA_INT,    id,
                "battery_ok",       "Battery",      DATA_INT,    !battery_low,
                "temperature_C",    "Temperature",  DATA_FORMAT, "%.1f C", DATA_DOUBLE, tempc,
                "humidity",         "Humidity",     DATA_FORMAT, "%u %%", DATA_INT,    humidity,
                "status",           "",             DATA_INT,    status,
                "mic",              "Integrity",    DATA_STRING, "CHECKSUM",
                NULL);
        /* clang-format on */

        decoder_output_data(decoder, data);
        valid++;
    }

    if (valid)
        return 1;

    // Only returns the latest result, but better than nothing.
    return result;
}