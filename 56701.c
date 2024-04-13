void ff_amf_write_field_name(uint8_t **dst, const char *str)
{
    bytestream_put_be16(dst, strlen(str));
    bytestream_put_buffer(dst, str, strlen(str));
}
