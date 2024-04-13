int ff_amf_read_number(GetByteContext *bc, double *val)
{
    uint64_t read;
    if (bytestream2_get_byte(bc) != AMF_DATA_TYPE_NUMBER)
        return AVERROR_INVALIDDATA;
    read = bytestream2_get_be64(bc);
    *val = av_int2double(read);
    return 0;
}
