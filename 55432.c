static int asf_read_language_list(AVFormatContext *s, int64_t size)
{
    AVIOContext *pb = s->pb;
    ASFContext *asf = s->priv_data;
    int j, ret;
    int stream_count = avio_rl16(pb);
    for (j = 0; j < stream_count; j++) {
        char lang[6];
        unsigned int lang_len = avio_r8(pb);
        if ((ret = avio_get_str16le(pb, lang_len, lang,
                                    sizeof(lang))) < lang_len)
            avio_skip(pb, lang_len - ret);
        if (j < 128)
            av_strlcpy(asf->stream_languages[j], lang,
                       sizeof(*asf->stream_languages));
    }

    return 0;
}
