static void write_real(bytearray_t * bplist, double val)
{
    int size = get_real_bytes(val);	//cheat to know used space
    uint8_t buff[9];
    buff[0] = BPLIST_REAL | Log2(size);
    if (size == sizeof(float)) {
        float floatval = (float)val;
        *(uint32_t*)(buff+1) = float_bswap32(*(uint32_t*)&floatval);
    } else {
        *(uint64_t*)(buff+1) = float_bswap64(*(uint64_t*)&val);
    }
    byte_array_append(bplist, buff, size+1);
}
