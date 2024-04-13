int ff_amf_read_null(GetByteContext *bc)
{
    if (bytestream2_get_byte(bc) != AMF_DATA_TYPE_NULL)
        return AVERROR_INVALIDDATA;
    return 0;
}
