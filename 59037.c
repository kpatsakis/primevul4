static void write_int(bytearray_t * bplist, uint64_t val)
{
    int size = get_needed_bytes(val);
    uint8_t sz;
    if (size == 3)
        size++;
    sz = BPLIST_UINT | Log2(size);

    val = be64toh(val);
    byte_array_append(bplist, &sz, 1);
    byte_array_append(bplist, (uint8_t*)&val + (8-size), size);
}
