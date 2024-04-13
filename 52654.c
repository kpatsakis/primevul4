static int mov_read_dac3(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    AVStream *st;
    int ac3info, acmod, lfeon, bsmod;

    if (c->fc->nb_streams < 1)
        return 0;
    st = c->fc->streams[c->fc->nb_streams-1];

    ac3info = avio_rb24(pb);
    bsmod = (ac3info >> 14) & 0x7;
    acmod = (ac3info >> 11) & 0x7;
    lfeon = (ac3info >> 10) & 0x1;
    st->codec->channels = ((int[]){2,1,2,3,3,4,4,5})[acmod] + lfeon;
    st->codec->channel_layout = avpriv_ac3_channel_layout_tab[acmod];
    if (lfeon)
        st->codec->channel_layout |= AV_CH_LOW_FREQUENCY;
    st->codec->audio_service_type = bsmod;
    if (st->codec->channels > 1 && bsmod == 0x7)
        st->codec->audio_service_type = AV_AUDIO_SERVICE_TYPE_KARAOKE;

    return 0;
}
