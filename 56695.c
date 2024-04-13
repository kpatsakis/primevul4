int ff_amf_read_bool(GetByteContext *bc, int *val)
{
    if (bytestream2_get_byte(bc) != AMF_DATA_TYPE_BOOL)
        return AVERROR_INVALIDDATA;
    *val = bytestream2_get_byte(bc);
    return 0;
}
