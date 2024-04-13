static int mov_read_frma(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    uint32_t format = avio_rl32(pb);
    MOVStreamContext *sc;
    enum AVCodecID id;
    AVStream *st;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams - 1];
    sc = st->priv_data;

    switch (sc->format)
    {
    case MKTAG('e','n','c','v'):        // encrypted video
    case MKTAG('e','n','c','a'):        // encrypted audio
        id = mov_codec_id(st, format);
        if (st->codecpar->codec_id != AV_CODEC_ID_NONE &&
            st->codecpar->codec_id != id) {
            av_log(c->fc, AV_LOG_WARNING,
                   "ignoring 'frma' atom of '%.4s', stream has codec id %d\n",
                   (char*)&format, st->codecpar->codec_id);
            break;
        }

        st->codecpar->codec_id = id;
        sc->format = format;
        break;

    default:
        if (format != sc->format) {
            av_log(c->fc, AV_LOG_WARNING,
                   "ignoring 'frma' atom of '%.4s', stream format is '%.4s'\n",
                   (char*)&format, (char*)&sc->format);
        }
        break;
    }

    return 0;
}
