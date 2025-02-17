static int asf_read_picture(AVFormatContext *s, int len)
{
    AVPacket pkt          = { 0 };
    const CodecMime *mime = ff_id3v2_mime_tags;
    enum  AVCodecID id    = AV_CODEC_ID_NONE;
    char mimetype[64];
    uint8_t  *desc = NULL;
    AVStream   *st = NULL;
    int ret, type, picsize, desc_len;

    /* type + picsize + mime + desc */
    if (len < 1 + 4 + 2 + 2) {
        av_log(s, AV_LOG_ERROR, "Invalid attached picture size: %d.\n", len);
        return AVERROR_INVALIDDATA;
    }

    /* picture type */
    type = avio_r8(s->pb);
    len--;
    if (type >= FF_ARRAY_ELEMS(ff_id3v2_picture_types) || type < 0) {
        av_log(s, AV_LOG_WARNING, "Unknown attached picture type: %d.\n", type);
        type = 0;
    }

    /* picture data size */
    picsize = avio_rl32(s->pb);
    len    -= 4;

    /* picture MIME type */
    len -= avio_get_str16le(s->pb, len, mimetype, sizeof(mimetype));
    while (mime->id != AV_CODEC_ID_NONE) {
        if (!strncmp(mime->str, mimetype, sizeof(mimetype))) {
            id = mime->id;
            break;
        }
        mime++;
    }
    if (id == AV_CODEC_ID_NONE) {
        av_log(s, AV_LOG_ERROR, "Unknown attached picture mimetype: %s.\n",
               mimetype);
        return 0;
    }

    if (picsize >= len) {
        av_log(s, AV_LOG_ERROR, "Invalid attached picture data size: %d >= %d.\n",
               picsize, len);
        return AVERROR_INVALIDDATA;
    }

    /* picture description */
    desc_len = (len - picsize) * 2 + 1;
    desc     = av_malloc(desc_len);
    if (!desc)
        return AVERROR(ENOMEM);
    len -= avio_get_str16le(s->pb, len - picsize, desc, desc_len);

    ret = av_get_packet(s->pb, &pkt, picsize);
    if (ret < 0)
        goto fail;

    st  = avformat_new_stream(s, NULL);
    if (!st) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    st->disposition              |= AV_DISPOSITION_ATTACHED_PIC;
    st->codecpar->codec_type      = AVMEDIA_TYPE_VIDEO;
    st->codecpar->codec_id        = id;
    st->attached_pic              = pkt;
    st->attached_pic.stream_index = st->index;
    st->attached_pic.flags       |= AV_PKT_FLAG_KEY;

    if (*desc)
        av_dict_set(&st->metadata, "title", desc, AV_DICT_DONT_STRDUP_VAL);
    else
        av_freep(&desc);

    av_dict_set(&st->metadata, "comment", ff_id3v2_picture_types[type], 0);

    return 0;

fail:
    av_freep(&desc);
    av_packet_unref(&pkt);
    return ret;
}
