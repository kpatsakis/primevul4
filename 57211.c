static int avi_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    AVIContext *avi = s->priv_data;
    AVIOContext *pb = s->pb;
    int err;

    if (CONFIG_DV_DEMUXER && avi->dv_demux) {
        int size = avpriv_dv_get_packet(avi->dv_demux, pkt);
        if (size >= 0)
            return size;
        else
            goto resync;
    }

    if (avi->non_interleaved) {
        err = ni_prepare_read(s);
        if (err < 0)
            return err;
    }

resync:
    if (avi->stream_index >= 0) {
        AVStream *st   = s->streams[avi->stream_index];
        AVIStream *ast = st->priv_data;
        int size, err;

        if (get_subtitle_pkt(s, st, pkt))
            return 0;

        if (ast->sample_size <= 1)
            size = INT_MAX;
        else if (ast->sample_size < 32)
            size = 1024 * ast->sample_size;
        else
            size = ast->sample_size;

        if (size > ast->remaining)
            size = ast->remaining;
        avi->last_pkt_pos = avio_tell(pb);
        err               = av_get_packet(pb, pkt, size);
        if (err < 0)
            return err;
        size = err;

        if (ast->has_pal && pkt->size < (unsigned)INT_MAX / 2) {
            uint8_t *pal;
            pal = av_packet_new_side_data(pkt,
                                          AV_PKT_DATA_PALETTE,
                                          AVPALETTE_SIZE);
            if (!pal) {
                av_log(s, AV_LOG_ERROR,
                       "Failed to allocate data for palette\n");
            } else {
                memcpy(pal, ast->pal, AVPALETTE_SIZE);
                ast->has_pal = 0;
            }
        }

        if (CONFIG_DV_DEMUXER && avi->dv_demux) {
            AVBufferRef *avbuf = pkt->buf;
            size = avpriv_dv_produce_packet(avi->dv_demux, pkt,
                                            pkt->data, pkt->size, pkt->pos);
            pkt->buf    = avbuf;
            pkt->flags |= AV_PKT_FLAG_KEY;
            if (size < 0)
                av_packet_unref(pkt);
        } else if (st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE &&
                   !st->codecpar->codec_tag && read_gab2_sub(s, st, pkt)) {
            ast->frame_offset++;
            avi->stream_index = -1;
            ast->remaining    = 0;
            goto resync;
        } else {
            /* XXX: How to handle B-frames in AVI? */
            pkt->dts = ast->frame_offset;
            if (ast->sample_size)
                pkt->dts /= ast->sample_size;
            pkt->stream_index = avi->stream_index;

            if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && st->index_entries) {
                AVIndexEntry *e;
                int index;

                index = av_index_search_timestamp(st, ast->frame_offset, AVSEEK_FLAG_ANY);
                e     = &st->index_entries[index];

                if (index >= 0 && e->timestamp == ast->frame_offset) {
                    if (index == st->nb_index_entries-1) {
                        int key=1;
                        uint32_t state=-1;
                        if (st->codecpar->codec_id == AV_CODEC_ID_MPEG4) {
                            const uint8_t *ptr = pkt->data, *end = ptr + FFMIN(size, 256);
                            while (ptr < end) {
                                ptr = avpriv_find_start_code(ptr, end, &state);
                                if (state == 0x1B6 && ptr < end) {
                                    key = !(*ptr & 0xC0);
                                    break;
                                }
                            }
                        }
                        if (!key)
                            e->flags &= ~AVINDEX_KEYFRAME;
                    }
                    if (e->flags & AVINDEX_KEYFRAME)
                        pkt->flags |= AV_PKT_FLAG_KEY;
                }
            } else {
                pkt->flags |= AV_PKT_FLAG_KEY;
            }
            ast->frame_offset += get_duration(ast, pkt->size);
        }
        ast->remaining -= err;
        if (!ast->remaining) {
            avi->stream_index = -1;
            ast->packet_size  = 0;
        }

        if (!avi->non_interleaved && pkt->pos >= 0 && ast->seek_pos > pkt->pos) {
            av_packet_unref(pkt);
            goto resync;
        }
        ast->seek_pos= 0;

        if (!avi->non_interleaved && st->nb_index_entries>1 && avi->index_loaded>1) {
            int64_t dts= av_rescale_q(pkt->dts, st->time_base, AV_TIME_BASE_Q);

            if (avi->dts_max - dts > 2*AV_TIME_BASE) {
                avi->non_interleaved= 1;
                av_log(s, AV_LOG_INFO, "Switching to NI mode, due to poor interleaving\n");
            }else if (avi->dts_max < dts)
                avi->dts_max = dts;
        }

        return 0;
    }

    if ((err = avi_sync(s, 0)) < 0)
        return err;
    goto resync;
}
