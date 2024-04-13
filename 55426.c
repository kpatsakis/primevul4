static int asf_read_content_desc(AVFormatContext *s, int64_t size)
{
    AVIOContext *pb = s->pb;
    int len1, len2, len3, len4, len5;

    len1 = avio_rl16(pb);
    len2 = avio_rl16(pb);
    len3 = avio_rl16(pb);
    len4 = avio_rl16(pb);
    len5 = avio_rl16(pb);
    get_tag(s, "title", 0, len1, 32);
    get_tag(s, "author", 0, len2, 32);
    get_tag(s, "copyright", 0, len3, 32);
    get_tag(s, "comment", 0, len4, 32);
    avio_skip(pb, len5);

    return 0;
}
