int ff_h264_parse_sprop_parameter_sets(AVFormatContext *s,
                                       uint8_t **data_ptr, int *size_ptr,
                                       const char *value)
{
    char base64packet[1024];
    uint8_t decoded_packet[1024];
    int packet_size;

    while (*value) {
        char *dst = base64packet;

        while (*value && *value != ','
               && (dst - base64packet) < sizeof(base64packet) - 1) {
            *dst++ = *value++;
        }
        *dst++ = '\0';

        if (*value == ',')
            value++;

        packet_size = av_base64_decode(decoded_packet, base64packet,
                                       sizeof(decoded_packet));
        if (packet_size > 0) {
            uint8_t *dest = av_realloc(*data_ptr,
                                       packet_size + sizeof(start_sequence) +
                                       *size_ptr +
                                       AV_INPUT_BUFFER_PADDING_SIZE);
            if (!dest) {
                av_log(s, AV_LOG_ERROR,
                       "Unable to allocate memory for extradata!\n");
                return AVERROR(ENOMEM);
            }
            *data_ptr = dest;

            memcpy(dest + *size_ptr, start_sequence,
                   sizeof(start_sequence));
            memcpy(dest + *size_ptr + sizeof(start_sequence),
                   decoded_packet, packet_size);
            memset(dest + *size_ptr + sizeof(start_sequence) +
                   packet_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);

            *size_ptr += sizeof(start_sequence) + packet_size;
        }
    }

    return 0;
}
