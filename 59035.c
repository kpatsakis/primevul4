static void write_data(bytearray_t * bplist, uint8_t * val, uint64_t size)
{
    write_raw_data(bplist, BPLIST_DATA, val, size);
}
