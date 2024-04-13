static int mov_codec_id(AVStream *st, uint32_t format)
{
    int id = ff_codec_get_id(ff_codec_movaudio_tags, format);

    if (id <= 0 &&
        ((format & 0xFFFF) == 'm' + ('s' << 8) ||
         (format & 0xFFFF) == 'T' + ('S' << 8)))
        id = ff_codec_get_id(ff_codec_wav_tags, av_bswap32(format) & 0xFFFF);

    if (st->codecpar->codec_type != AVMEDIA_TYPE_VIDEO && id > 0) {
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    } else if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
               /* skip old ASF MPEG-4 tag */
               format && format != MKTAG('m','p','4','s')) {
        id = ff_codec_get_id(ff_codec_movvideo_tags, format);
        if (id <= 0)
            id = ff_codec_get_id(ff_codec_bmp_tags, format);
        if (id > 0)
            st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        else if (st->codecpar->codec_type == AVMEDIA_TYPE_DATA ||
                    (st->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE &&
                    st->codecpar->codec_id == AV_CODEC_ID_NONE)) {
            id = ff_codec_get_id(ff_codec_movsubtitle_tags, format);
            if (id > 0)
                st->codecpar->codec_type = AVMEDIA_TYPE_SUBTITLE;
        }
    }

    st->codecpar->codec_tag = format;

    return id;
}
