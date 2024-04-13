static int avi_load_index(AVFormatContext *s)
{
    AVIContext *avi = s->priv_data;
    AVIOContext *pb = s->pb;
    uint32_t tag, size;
    int64_t pos = avio_tell(pb);
    int64_t next;
    int ret     = -1;

    if (avio_seek(pb, avi->movi_end, SEEK_SET) < 0)
        goto the_end; // maybe truncated file
    av_log(s, AV_LOG_TRACE, "movi_end=0x%"PRIx64"\n", avi->movi_end);
    for (;;) {
        tag  = avio_rl32(pb);
        size = avio_rl32(pb);
        if (avio_feof(pb))
            break;
        next = avio_tell(pb) + size + (size & 1);

        if (tag == MKTAG('i', 'd', 'x', '1') &&
            avi_read_idx1(s, size) >= 0) {
            avi->index_loaded=2;
            ret = 0;
        }else if (tag == MKTAG('L', 'I', 'S', 'T')) {
            uint32_t tag1 = avio_rl32(pb);

            if (tag1 == MKTAG('I', 'N', 'F', 'O'))
                ff_read_riff_info(s, size - 4);
        }else if (!ret)
            break;

        if (avio_seek(pb, next, SEEK_SET) < 0)
            break; // something is wrong here
    }

the_end:
    avio_seek(pb, pos, SEEK_SET);
    return ret;
}
