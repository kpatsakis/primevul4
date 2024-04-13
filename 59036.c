static void write_date(bytearray_t * bplist, double val)
{
    uint8_t buff[9];
    buff[0] = BPLIST_DATE | 3;
    *(uint64_t*)(buff+1) = float_bswap64(*(uint64_t*)&val);
    byte_array_append(bplist, buff, sizeof(buff));
}
