static int mxf_absolute_bodysid_offset(MXFContext *mxf, int body_sid, int64_t offset, int64_t *offset_out)
{
    int x;
    int64_t offset_in = offset;     /* for logging */

    for (x = 0; x < mxf->partitions_count; x++) {
        MXFPartition *p = &mxf->partitions[x];

        if (p->body_sid != body_sid)
            continue;

        if (offset < p->essence_length || !p->essence_length) {
            *offset_out = p->essence_offset + offset;
            return 0;
        }

        offset -= p->essence_length;
    }

    av_log(mxf->fc, AV_LOG_ERROR,
           "failed to find absolute offset of %"PRIX64" in BodySID %i - partial file?\n",
           offset_in, body_sid);

    return AVERROR_INVALIDDATA;
}
