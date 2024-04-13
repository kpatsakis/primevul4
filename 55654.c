static int mxf_read_strong_ref_array(AVIOContext *pb, UID **refs, int *count)
{
    *count = avio_rb32(pb);
    *refs = av_calloc(*count, sizeof(UID));
    if (!*refs) {
        *count = 0;
        return AVERROR(ENOMEM);
    }
    avio_skip(pb, 4); /* useless size of objects, always 16 according to specs */
    avio_read(pb, (uint8_t *)*refs, *count * sizeof(UID));
    return 0;
}
