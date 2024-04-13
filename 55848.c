static int ivr_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    RMDemuxContext *rm = s->priv_data;
    int ret = AVERROR_EOF, opcode;
    AVIOContext *pb = s->pb;
    unsigned size, index;
    int64_t pos, pts;

    if (avio_feof(pb) || rm->data_end)
        return AVERROR_EOF;

    pos = avio_tell(pb);

    for (;;) {
        if (rm->audio_pkt_cnt) {
            AVStream *st;

            st = s->streams[rm->audio_stream_num];
            ret = ff_rm_retrieve_cache(s, pb, st, st->priv_data, pkt);
            if (ret < 0) {
                return ret;
            }
        } else {
            if (rm->remaining_len) {
                avio_skip(pb, rm->remaining_len);
                rm->remaining_len = 0;
            }

            if (avio_feof(pb))
                return AVERROR_EOF;

            opcode = avio_r8(pb);
            if (opcode == 2) {
                AVStream *st;
                int seq = 1;

                pts = avio_rb32(pb);
                index = avio_rb16(pb);
                if (index >= s->nb_streams)
                    return AVERROR_INVALIDDATA;

                avio_skip(pb, 4);
                size = avio_rb32(pb);
                avio_skip(pb, 4);

                if (size < 1 || size > INT_MAX/4) {
                    av_log(s, AV_LOG_ERROR, "size %u is invalid\n", size);
                    return AVERROR_INVALIDDATA;
                }

                st = s->streams[index];
                ret = ff_rm_parse_packet(s, pb, st, st->priv_data, size, pkt,
                                         &seq, 0, pts);
                if (ret < -1) {
                    return ret;
                } else if (ret) {
                    continue;
                }

                pkt->pos = pos;
                pkt->pts = pts;
                pkt->stream_index = index;
            } else if (opcode == 7) {
                pos = avio_rb64(pb);
                if (!pos) {
                    rm->data_end = 1;
                    return AVERROR_EOF;
                }
            } else {
                av_log(s, AV_LOG_ERROR, "Unsupported opcode=%d at %"PRIX64"\n", opcode, avio_tell(pb) - 1);
                return AVERROR(EIO);
            }
        }

        break;
    }

    return ret;
}
