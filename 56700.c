void ff_amf_write_bool(uint8_t **dst, int val)
{
    bytestream_put_byte(dst, AMF_DATA_TYPE_BOOL);
    bytestream_put_byte(dst, val);
}
