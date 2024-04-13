static void skip_to_key(AVFormatContext *s)
{
    ASFContext *asf = s->priv_data;
    int i;

    for (i = 0; i < 128; i++) {
        int j = asf->asfid2avid[i];
        ASFStream *asf_st = &asf->streams[i];
        if (j < 0 || s->streams[j]->codecpar->codec_type != AVMEDIA_TYPE_VIDEO)
            continue;

        asf_st->skip_to_key = 1;
    }
}
