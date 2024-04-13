int ff_amf_get_string(GetByteContext *bc, uint8_t *str,
                      int strsize, int *length)
{
    int stringlen = 0;
    int readsize;
    stringlen = bytestream2_get_be16(bc);
    if (stringlen + 1 > strsize)
        return AVERROR(EINVAL);
    readsize = bytestream2_get_buffer(bc, str, stringlen);
    if (readsize != stringlen) {
        av_log(NULL, AV_LOG_WARNING,
               "Unable to read as many bytes as AMF string signaled\n");
    }
    str[readsize] = '\0';
    *length = FFMIN(stringlen, readsize);
    return 0;
}
