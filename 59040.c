static void write_string(bytearray_t * bplist, char *val)
{
    uint64_t size = strlen(val);
    write_raw_data(bplist, BPLIST_STRING, (uint8_t *) val, size);
}
