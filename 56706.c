void ff_amf_write_string2(uint8_t **dst, const char *str1, const char *str2)
{
    int len1 = 0, len2 = 0;
    if (str1)
        len1 = strlen(str1);
    if (str2)
        len2 = strlen(str2);
    bytestream_put_byte(dst, AMF_DATA_TYPE_STRING);
    bytestream_put_be16(dst, len1 + len2);
    bytestream_put_buffer(dst, str1, len1);
    bytestream_put_buffer(dst, str2, len2);
}
