int ff_amf_read_string(GetByteContext *bc, uint8_t *str,
                       int strsize, int *length)
{
    if (bytestream2_get_byte(bc) != AMF_DATA_TYPE_STRING)
        return AVERROR_INVALIDDATA;
    return ff_amf_get_string(bc, str, strsize, length);
}
