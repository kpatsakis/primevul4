static int parse_timecode_in_framenum_format(AVFormatContext *s, AVStream *st,
                                             uint32_t value, int flags)
{
    AVTimecode tc;
    char buf[AV_TIMECODE_STR_SIZE];
    AVRational rate = {st->codec->time_base.den,
                       st->codec->time_base.num};
    int ret = av_timecode_init(&tc, rate, flags, 0, s);
    if (ret < 0)
        return ret;
    av_dict_set(&st->metadata, "timecode",
                av_timecode_make_string(&tc, buf, value), 0);
    return 0;
}
