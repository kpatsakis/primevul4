static int mov_read_dac3(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    enum AVAudioServiceType *ast;
    int ac3info, acmod, lfeon, bsmod;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    ast = (enum AVAudioServiceType*)av_stream_new_side_data(st, AV_PKT_DATA_AUDIO_SERVICE_TYPE,
                                                            sizeof(*ast));
    if (!ast)
        return AVERROR(ENOMEM);

    ac3info = avio_rb24(pb);
    bsmod = (ac3info >> 14) & 0x7;
    acmod = (ac3info >> 11) & 0x7;
    lfeon = (ac3info >> 10) & 0x1;
    st->codecpar->channels = ((int[]){2,1,2,3,3,4,4,5})[acmod] + lfeon;
    st->codecpar->channel_layout = avpriv_ac3_channel_layout_tab[acmod];
    if (lfeon)
        st->codecpar->channel_layout |= AV_CH_LOW_FREQUENCY;
    *ast = bsmod;
    if (st->codecpar->channels > 1 && bsmod == 0x7)
        *ast = AV_AUDIO_SERVICE_TYPE_KARAOKE;

#if FF_API_LAVF_AVCTX
    FF_DISABLE_DEPRECATION_WARNINGS
    st->codec->audio_service_type = *ast;
    FF_ENABLE_DEPRECATION_WARNINGS
#endif

    return 0;
}
