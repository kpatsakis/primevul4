static int avi_read_idx1(AVFormatContext *s, int size)
{
    AVIContext *avi = s->priv_data;
    AVIOContext *pb = s->pb;
    int nb_index_entries, i;
    AVStream *st;
    AVIStream *ast;
    int64_t pos;
    unsigned int index, tag, flags, len, first_packet = 1;
    int64_t last_pos = -1;
    unsigned last_idx = -1;
    int64_t idx1_pos, first_packet_pos = 0, data_offset = 0;
    int anykey = 0;

    nb_index_entries = size / 16;
    if (nb_index_entries <= 0)
        return AVERROR_INVALIDDATA;

    idx1_pos = avio_tell(pb);
    avio_seek(pb, avi->movi_list + 4, SEEK_SET);
    if (avi_sync(s, 1) == 0)
        first_packet_pos = avio_tell(pb) - 8;
    avi->stream_index = -1;
    avio_seek(pb, idx1_pos, SEEK_SET);

    if (s->nb_streams == 1 && s->streams[0]->codecpar->codec_tag == AV_RL32("MMES")) {
        first_packet_pos = 0;
        data_offset = avi->movi_list;
    }

    /* Read the entries and sort them in each stream component. */
    for (i = 0; i < nb_index_entries; i++) {
        if (avio_feof(pb))
            return -1;

        tag   = avio_rl32(pb);
        flags = avio_rl32(pb);
        pos   = avio_rl32(pb);
        len   = avio_rl32(pb);
        av_log(s, AV_LOG_TRACE, "%d: tag=0x%x flags=0x%x pos=0x%"PRIx64" len=%d/",
                i, tag, flags, pos, len);

        index  = ((tag      & 0xff) - '0') * 10;
        index +=  (tag >> 8 & 0xff) - '0';
        if (index >= s->nb_streams)
            continue;
        st  = s->streams[index];
        ast = st->priv_data;

        /* Skip 'xxpc' palette change entries in the index until a logic
         * to process these is properly implemented. */
        if ((tag >> 16 & 0xff) == 'p' && (tag >> 24 & 0xff) == 'c')
            continue;

        if (first_packet && first_packet_pos) {
            if (avi->movi_list + 4 != pos || pos + 500 > first_packet_pos)
                data_offset  = first_packet_pos - pos;
            first_packet = 0;
        }
        pos += data_offset;

        av_log(s, AV_LOG_TRACE, "%d cum_len=%"PRId64"\n", len, ast->cum_len);

        if (last_pos == pos)
            avi->non_interleaved = 1;
        if (last_idx != pos && len) {
            av_add_index_entry(st, pos, ast->cum_len, len, 0,
                               (flags & AVIIF_INDEX) ? AVINDEX_KEYFRAME : 0);
            last_idx= pos;
        }
        ast->cum_len += get_duration(ast, len);
        last_pos      = pos;
        anykey       |= flags&AVIIF_INDEX;
    }
    if (!anykey) {
        for (index = 0; index < s->nb_streams; index++) {
            st = s->streams[index];
            if (st->nb_index_entries)
                st->index_entries[0].flags |= AVINDEX_KEYFRAME;
        }
    }
    return 0;
}
