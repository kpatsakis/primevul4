static int mxf_parse_klv(MXFContext *mxf, KLVPacket klv, MXFMetadataReadFunc *read,
                                     int ctx_size, enum MXFMetadataSetType type)
{
    AVFormatContext *s = mxf->fc;
    int res;
    if (klv.key[5] == 0x53) {
        res = mxf_read_local_tags(mxf, &klv, read, ctx_size, type);
    } else {
        uint64_t next = avio_tell(s->pb) + klv.length;
        res = read(mxf, s->pb, 0, klv.length, klv.key, klv.offset);

        /* only seek forward, else this can loop for a long time */
        if (avio_tell(s->pb) > next) {
            av_log(s, AV_LOG_ERROR, "read past end of KLV @ %#"PRIx64"\n",
                   klv.offset);
            return AVERROR_INVALIDDATA;
        }

        avio_seek(s->pb, next, SEEK_SET);
    }
    if (res < 0) {
        av_log(s, AV_LOG_ERROR, "error reading header metadata\n");
        return res;
    }
    return 0;
}
