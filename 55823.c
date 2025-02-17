static int asf_build_simple_index(AVFormatContext *s, int stream_index)
{
    ff_asf_guid g;
    ASFContext *asf     = s->priv_data;
    int64_t current_pos = avio_tell(s->pb);
    int64_t ret;

    if((ret = avio_seek(s->pb, asf->data_object_offset + asf->data_object_size, SEEK_SET)) < 0) {
        return ret;
    }

    if ((ret = ff_get_guid(s->pb, &g)) < 0)
        goto end;

    /* the data object can be followed by other top-level objects,
     * skip them until the simple index object is reached */
    while (ff_guidcmp(&g, &ff_asf_simple_index_header)) {
        int64_t gsize = avio_rl64(s->pb);
        if (gsize < 24 || avio_feof(s->pb)) {
            goto end;
        }
        avio_skip(s->pb, gsize - 24);
        if ((ret = ff_get_guid(s->pb, &g)) < 0)
            goto end;
    }

    {
        int64_t itime, last_pos = -1;
        int pct, ict;
        int i;
        int64_t av_unused gsize = avio_rl64(s->pb);
        if ((ret = ff_get_guid(s->pb, &g)) < 0)
            goto end;
        itime = avio_rl64(s->pb);
        pct   = avio_rl32(s->pb);
        ict   = avio_rl32(s->pb);
        av_log(s, AV_LOG_DEBUG,
               "itime:0x%"PRIx64", pct:%d, ict:%d\n", itime, pct, ict);

        for (i = 0; i < ict; i++) {
            int pktnum        = avio_rl32(s->pb);
            int pktct         = avio_rl16(s->pb);
            int64_t pos       = s->internal->data_offset + s->packet_size * (int64_t)pktnum;
            int64_t index_pts = FFMAX(av_rescale(itime, i, 10000) - asf->hdr.preroll, 0);

            if (pos != last_pos) {
                av_log(s, AV_LOG_DEBUG, "pktnum:%d, pktct:%d  pts: %"PRId64"\n",
                       pktnum, pktct, index_pts);
                av_add_index_entry(s->streams[stream_index], pos, index_pts,
                                   s->packet_size, 0, AVINDEX_KEYFRAME);
                last_pos = pos;
            }
        }
        asf->index_read = ict > 1;
    }
end:
    avio_seek(s->pb, current_pos, SEEK_SET);
    return ret;
}
