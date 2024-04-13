static int mov_read_enda(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int little_endian;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    little_endian = avio_rb16(pb) & 0xFF;
    av_log(c->fc, AV_LOG_TRACE, "enda %d\n", little_endian);
    if (little_endian == 1) {
        switch (st->codecpar->codec_id) {
        case AV_CODEC_ID_PCM_S24BE:
            st->codecpar->codec_id = AV_CODEC_ID_PCM_S24LE;
            break;
        case AV_CODEC_ID_PCM_S32BE:
            st->codecpar->codec_id = AV_CODEC_ID_PCM_S32LE;
            break;
        case AV_CODEC_ID_PCM_F32BE:
            st->codecpar->codec_id = AV_CODEC_ID_PCM_F32LE;
            break;
        case AV_CODEC_ID_PCM_F64BE:
            st->codecpar->codec_id = AV_CODEC_ID_PCM_F64LE;
            break;
        default:
            break;
        }
    }
    return 0;
}
