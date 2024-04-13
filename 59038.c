static void write_raw_data(bytearray_t * bplist, uint8_t mark, uint8_t * val, uint64_t size)
{
    uint8_t marker = mark | (size < 15 ? size : 0xf);
    byte_array_append(bplist, &marker, sizeof(uint8_t));
    if (size >= 15) {
        write_int(bplist, size);
    }
    if (BPLIST_UNICODE==mark) size <<= 1;
    byte_array_append(bplist, val, size);
}
