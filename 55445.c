static void export_orphan_timecode(AVFormatContext *s)
{
    int i;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];

        if (st->codecpar->codec_tag  == MKTAG('t','m','c','d') &&
            !tmcd_is_referenced(s, i + 1)) {
            AVDictionaryEntry *tcr = av_dict_get(st->metadata, "timecode", NULL, 0);
            if (tcr) {
                av_dict_set(&s->metadata, "timecode", tcr->value, 0);
                break;
            }
        }
    }
}
