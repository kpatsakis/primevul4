int ff_mov_read_stsd_entries(MOVContext *c, AVIOContext *pb, int entries)
{
    AVStream *st;
    MOVStreamContext *sc;
    int pseudo_stream_id;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];
    sc = st->priv_data;

    for (pseudo_stream_id = 0;
         pseudo_stream_id < entries && !pb->eof_reached;
         pseudo_stream_id++) {
        enum AVCodecID id;
        int ret, dref_id = 1;
        MOVAtom a = { AV_RL32("stsd") };
        int64_t start_pos = avio_tell(pb);
        int64_t size    = avio_rb32(pb); /* size */
        uint32_t format = avio_rl32(pb); /* data format */

        if (size >= 16) {
            avio_rb32(pb); /* reserved */
            avio_rb16(pb); /* reserved */
            dref_id = avio_rb16(pb);
        } else if (size <= 7) {
            av_log(c->fc, AV_LOG_ERROR,
                   "invalid size %"PRId64" in stsd\n", size);
            return AVERROR_INVALIDDATA;
        }

        if (mov_skip_multiple_stsd(c, pb, st->codecpar->codec_tag, format,
                                   size - (avio_tell(pb) - start_pos)))
            continue;

        sc->pseudo_stream_id = st->codecpar->codec_tag ? -1 : pseudo_stream_id;
        sc->dref_id= dref_id;
        sc->format = format;

        id = mov_codec_id(st, format);

        av_log(c->fc, AV_LOG_TRACE,
               "size=%"PRId64" 4CC=%s codec_type=%d\n", size,
               av_fourcc2str(format), st->codecpar->codec_type);

        if (st->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
            st->codecpar->codec_id = id;
            mov_parse_stsd_video(c, pb, st, sc);
        } else if (st->codecpar->codec_type==AVMEDIA_TYPE_AUDIO) {
            st->codecpar->codec_id = id;
            mov_parse_stsd_audio(c, pb, st, sc);
            if (st->codecpar->sample_rate < 0) {
                av_log(c->fc, AV_LOG_ERROR, "Invalid sample rate %d\n", st->codecpar->sample_rate);
                return AVERROR_INVALIDDATA;
            }
        } else if (st->codecpar->codec_type==AVMEDIA_TYPE_SUBTITLE){
            st->codecpar->codec_id = id;
            mov_parse_stsd_subtitle(c, pb, st, sc,
                                    size - (avio_tell(pb) - start_pos));
        } else {
            ret = mov_parse_stsd_data(c, pb, st, sc,
                                      size - (avio_tell(pb) - start_pos));
            if (ret < 0)
                return ret;
        }
        /* this will read extra atoms at the end (wave, alac, damr, avcC, hvcC, SMI ...) */
        a.size = size - (avio_tell(pb) - start_pos);
        if (a.size > 8) {
            if ((ret = mov_read_default(c, pb, a)) < 0)
                return ret;
        } else if (a.size > 0)
            avio_skip(pb, a.size);

        if (sc->extradata && st->codecpar->extradata) {
            int extra_size = st->codecpar->extradata_size;

            /* Move the current stream extradata to the stream context one. */
            sc->extradata_size[pseudo_stream_id] = extra_size;
            sc->extradata[pseudo_stream_id] = av_malloc(extra_size + AV_INPUT_BUFFER_PADDING_SIZE);
            if (!sc->extradata[pseudo_stream_id])
                return AVERROR(ENOMEM);
            memcpy(sc->extradata[pseudo_stream_id], st->codecpar->extradata, extra_size);
            av_freep(&st->codecpar->extradata);
            st->codecpar->extradata_size = 0;
        }
    }

    if (pb->eof_reached)
        return AVERROR_EOF;

    return 0;
}
