static int asf_read_ext_content_desc(AVFormatContext *s, int64_t size)
{
    AVIOContext *pb = s->pb;
    ASFContext *asf = s->priv_data;
    int desc_count, i, ret;

    desc_count = avio_rl16(pb);
    for (i = 0; i < desc_count; i++) {
        int name_len, value_type, value_len;
        char name[1024];

        name_len = avio_rl16(pb);
        if (name_len % 2)   // must be even, broken lavf versions wrote len-1
            name_len += 1;
        if ((ret = avio_get_str16le(pb, name_len, name, sizeof(name))) < name_len)
            avio_skip(pb, name_len - ret);
        value_type = avio_rl16(pb);
        value_len  = avio_rl16(pb);
        if (!value_type && value_len % 2)
            value_len += 1;
        /* My sample has that stream set to 0 maybe that mean the container.
         * ASF stream count starts at 1. I am using 0 to the container value
         * since it's unused. */
        if (!strcmp(name, "AspectRatioX"))
            asf->dar[0].num = get_value(s->pb, value_type, 32);
        else if (!strcmp(name, "AspectRatioY"))
            asf->dar[0].den = get_value(s->pb, value_type, 32);
        else
            get_tag(s, name, value_type, value_len, 32);
    }

    return 0;
}
