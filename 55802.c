static int hls_read_seek(AVFormatContext *s, int stream_index,
                               int64_t timestamp, int flags)
{
    HLSContext *c = s->priv_data;
    struct playlist *seek_pls = NULL;
    int i, seq_no;
    int j;
    int stream_subdemuxer_index;
    int64_t first_timestamp, seek_timestamp, duration;

    if ((flags & AVSEEK_FLAG_BYTE) ||
        !(c->variants[0]->playlists[0]->finished || c->variants[0]->playlists[0]->type == PLS_TYPE_EVENT))
        return AVERROR(ENOSYS);

    first_timestamp = c->first_timestamp == AV_NOPTS_VALUE ?
                      0 : c->first_timestamp;

    seek_timestamp = av_rescale_rnd(timestamp, AV_TIME_BASE,
                                    s->streams[stream_index]->time_base.den,
                                    flags & AVSEEK_FLAG_BACKWARD ?
                                    AV_ROUND_DOWN : AV_ROUND_UP);

    duration = s->duration == AV_NOPTS_VALUE ?
               0 : s->duration;

    if (0 < duration && duration < seek_timestamp - first_timestamp)
        return AVERROR(EIO);

    /* find the playlist with the specified stream */
    for (i = 0; i < c->n_playlists; i++) {
        struct playlist *pls = c->playlists[i];
        for (j = 0; j < pls->n_main_streams; j++) {
            if (pls->main_streams[j] == s->streams[stream_index]) {
                seek_pls = pls;
                stream_subdemuxer_index = j;
                break;
            }
        }
    }
    /* check if the timestamp is valid for the playlist with the
     * specified stream index */
    if (!seek_pls || !find_timestamp_in_playlist(c, seek_pls, seek_timestamp, &seq_no))
        return AVERROR(EIO);

    /* set segment now so we do not need to search again below */
    seek_pls->cur_seq_no = seq_no;
    seek_pls->seek_stream_index = stream_subdemuxer_index;

    for (i = 0; i < c->n_playlists; i++) {
        /* Reset reading */
        struct playlist *pls = c->playlists[i];
        if (pls->input)
            ff_format_io_close(pls->parent, &pls->input);
        av_packet_unref(&pls->pkt);
        reset_packet(&pls->pkt);
        pls->pb.eof_reached = 0;
        /* Clear any buffered data */
        pls->pb.buf_end = pls->pb.buf_ptr = pls->pb.buffer;
        /* Reset the pos, to let the mpegts demuxer know we've seeked. */
        pls->pb.pos = 0;
        /* Flush the packet queue of the subdemuxer. */
        ff_read_frame_flush(pls->ctx);

        pls->seek_timestamp = seek_timestamp;
        pls->seek_flags = flags;

        if (pls != seek_pls) {
            /* set closest segment seq_no for playlists not handled above */
            find_timestamp_in_playlist(c, pls, seek_timestamp, &pls->cur_seq_no);
            /* seek the playlist to the given position without taking
             * keyframes into account since this playlist does not have the
             * specified stream where we should look for the keyframes */
            pls->seek_stream_index = -1;
            pls->seek_flags |= AVSEEK_FLAG_ANY;
        }
    }

    c->cur_timestamp = seek_timestamp;

    return 0;
}
