static int mxf_is_partition_pack_key(UID key)
{
    return !memcmp(key, mxf_header_partition_pack_key, 13) &&
            key[13] >= 2 && key[13] <= 4;
}
