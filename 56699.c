int ff_amf_tag_size(const uint8_t *data, const uint8_t *data_end)
{
    GetByteContext gb;
    int ret;

    if (data >= data_end)
        return -1;

    bytestream2_init(&gb, data, data_end - data);

    ret = amf_tag_skip(&gb);
    if (ret < 0 || bytestream2_get_bytes_left(&gb) <= 0)
        return -1;
    av_assert0(bytestream2_tell(&gb) >= 0 && bytestream2_tell(&gb) <= data_end - data);
     return bytestream2_tell(&gb);
 }
