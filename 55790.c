static void fill_timing_for_id3_timestamped_stream(struct playlist *pls)
{
    if (pls->id3_offset >= 0) {
        pls->pkt.dts = pls->id3_mpegts_timestamp +
                                 av_rescale_q(pls->id3_offset,
                                              pls->ctx->streams[pls->pkt.stream_index]->time_base,
                                              MPEG_TIME_BASE_Q);
        if (pls->pkt.duration)
            pls->id3_offset += pls->pkt.duration;
        else
            pls->id3_offset = -1;
    } else {
        /* there have been packets with unknown duration
         * since the last id3 tag, should not normally happen */
        pls->pkt.dts = AV_NOPTS_VALUE;
    }

    if (pls->pkt.duration)
        pls->pkt.duration = av_rescale_q(pls->pkt.duration,
                                         pls->ctx->streams[pls->pkt.stream_index]->time_base,
                                         MPEG_TIME_BASE_Q);

    pls->pkt.pts = AV_NOPTS_VALUE;
}
