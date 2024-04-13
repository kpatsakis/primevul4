static int acurite_986_decode(r_device *decoder, bitbuffer_t *bitbuffer)
{
    int const browlen = 5;
    uint8_t *bb, sensor_num, status, crc, crcc;
    uint8_t br[8];
    int8_t tempf; // Raw Temp is 8 bit signed Fahrenheit
    uint16_t sensor_id, valid_cnt = 0;
    char sensor_type;
    char *channel_str;
    int battery_low;
    data_t *data;

    int result = 0;

    for (uint16_t brow = 0; brow < bitbuffer->num_rows; ++brow) {

        decoder_logf(decoder, 2, __func__, "row %u bits %u, bytes %d", brow, bitbuffer->bits_per_row[brow], browlen);

        if (bitbuffer->bits_per_row[brow] < 39 ||
            bitbuffer->bits_per_row[brow] > 43 ) {
            if (bitbuffer->bits_per_row[brow] > 16)
                decoder_log(decoder, 2, __func__,"skipping wrong len");
            result = DECODE_ABORT_LENGTH;
            continue; // DECODE_ABORT_LENGTH
        }
        bb = bitbuffer->bb[brow];

        // Reduce false positives
        // may eliminate these with a better PPM (precise?) demod.
        if ((bb[0] == 0xff && bb[1] == 0xff && bb[2] == 0xff) ||
                (bb[0] == 0x00 && bb[1] == 0x00 && bb[2] == 0x00)) {
            result = DECODE_ABORT_EARLY;
            continue; // DECODE_ABORT_EARLY
        }

        // Reverse the bits, msg sent LSB first
        for (int i = 0; i < browlen; i++)
            br[i] = reverse8(bb[i]);

        decoder_log_bitrow(decoder, 1, __func__, br, browlen * 8, "reversed");

        tempf = br[0];
        sensor_id = (br[1] << 8) + br[2];
        status = br[3];
        sensor_num = (status & 0x01) + 1;
        status = status >> 1;
        battery_low = ((status & 1) == 1);

        // By default Sensor 1 is the Freezer, 2 Refrigerator
        sensor_type = sensor_num == 2 ? 'F' : 'R';
        channel_str = sensor_num == 2 ? "2F" : "1R";

        crc = br[4];
        crcc = crc8le(br, 4, 0x07, 0);

        if (crcc != crc) {
            decoder_logf_bitrow(decoder, 2, __func__, br, browlen * 8, "bad CRC: %02x -", crc8le(br, 4, 0x07, 0));
            // HACK: rct 2018-04-22
            // the message is often missing the last 1 bit either due to a
            // problem with the device or demodulator
            // Add 1 (0x80 because message is LSB) and retry CRC.
            if (crcc == (crc | 0x80)) {
                decoder_logf(decoder, 2, __func__, "CRC fix %02x - %02x", crc, crcc);
            }
            else {
                continue; // DECODE_FAIL_MIC
            }
        }

        if (tempf & 0x80) {
            tempf = (tempf & 0x7f) * -1;
        }

        decoder_logf(decoder, 1, __func__, "sensor 0x%04x - %d%c: %d F", sensor_id, sensor_num, sensor_type, tempf);

        /* clang-format off */
        data = data_make(
                "model",            "",             DATA_STRING, "Acurite-986",
                "id",               NULL,           DATA_INT,    sensor_id,
                "channel",          NULL,           DATA_STRING, channel_str,
                "battery_ok",       "Battery",      DATA_INT,    !battery_low,
                "temperature_F",    "temperature",  DATA_FORMAT, "%f F", DATA_DOUBLE,    (float)tempf,
                "status",           "status",       DATA_INT,    status,
                "mic",              "Integrity",    DATA_STRING, "CRC",
                NULL);
        /* clang-format on */

        decoder_output_data(decoder, data);

        valid_cnt++;
    }

    if (valid_cnt)
        return 1;

    return result;
}