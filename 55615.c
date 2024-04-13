static int64_t mxf_essence_container_end(MXFContext *mxf, int body_sid)
{
    int x;
    int64_t ret = 0;

    for (x = 0; x < mxf->partitions_count; x++) {
        MXFPartition *p = &mxf->partitions[x];

        if (p->body_sid != body_sid)
            continue;

        if (!p->essence_length)
            return 0;

        ret = p->essence_offset + p->essence_length;
    }

    return ret;
}
