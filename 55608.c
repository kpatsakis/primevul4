static int mxf_add_timecode_metadata(AVDictionary **pm, const char *key, AVTimecode *tc)
{
    char buf[AV_TIMECODE_STR_SIZE];
    av_dict_set(pm, key, av_timecode_make_string(tc, buf, 0), 0);

    return 0;
}
