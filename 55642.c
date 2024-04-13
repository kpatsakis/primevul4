static int mxf_read_indirect_value(void *arg, AVIOContext *pb, int size)
{
    MXFTaggedValue *tagged_value = arg;
    uint8_t key[17];

    if (size <= 17)
        return 0;

    avio_read(pb, key, 17);
    /* TODO: handle other types of of indirect values */
    if (memcmp(key, mxf_indirect_value_utf16le, 17) == 0) {
        return mxf_read_utf16le_string(pb, size - 17, &tagged_value->value);
    } else if (memcmp(key, mxf_indirect_value_utf16be, 17) == 0) {
        return mxf_read_utf16be_string(pb, size - 17, &tagged_value->value);
    }
    return 0;
}
