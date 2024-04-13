static int tmcd_is_referenced(AVFormatContext *s, int tmcd_id)
{
    int i;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        MOVStreamContext *sc = st->priv_data;

        if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO &&
            sc->timecode_track == tmcd_id)
            return 1;
    }
    return 0;
}
