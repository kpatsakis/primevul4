static int asf_read_metadata(AVFormatContext *s, int64_t size)
{
    AVIOContext *pb = s->pb;
    ASFContext *asf = s->priv_data;
    int n, stream_num, name_len_utf16, name_len_utf8, value_len;
    int ret, i;
    n = avio_rl16(pb);

    for (i = 0; i < n; i++) {
        uint8_t *name;
        int value_type;

        avio_rl16(pb);  // lang_list_index
        stream_num = avio_rl16(pb);
        name_len_utf16 = avio_rl16(pb);
        value_type = avio_rl16(pb); /* value_type */
        value_len  = avio_rl32(pb);

        name_len_utf8 = 2*name_len_utf16 + 1;
        name          = av_malloc(name_len_utf8);
        if (!name)
            return AVERROR(ENOMEM);

        if ((ret = avio_get_str16le(pb, name_len_utf16, name, name_len_utf8)) < name_len_utf16)
            avio_skip(pb, name_len_utf16 - ret);
        av_log(s, AV_LOG_TRACE, "%d stream %d name_len %2d type %d len %4d <%s>\n",
                i, stream_num, name_len_utf16, value_type, value_len, name);

        if (!strcmp(name, "AspectRatioX")){
            int aspect_x = get_value(s->pb, value_type, 16);
            if(stream_num < 128)
                asf->dar[stream_num].num = aspect_x;
        } else if(!strcmp(name, "AspectRatioY")){
            int aspect_y = get_value(s->pb, value_type, 16);
            if(stream_num < 128)
                asf->dar[stream_num].den = aspect_y;
        } else {
            get_tag(s, name, value_type, value_len, 16);
        }
        av_freep(&name);
    }

    return 0;
}
