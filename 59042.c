static void write_unicode(bytearray_t * bplist, uint16_t * val, uint64_t size)
{
    uint64_t i = 0;
    uint16_t *buff = (uint16_t*)malloc(size << 1);
    for (i = 0; i < size; i++)
        buff[i] = be16toh(val[i]);
    write_raw_data(bplist, BPLIST_UNICODE, (uint8_t*)buff, size);
    free(buff);
}
