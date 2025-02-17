static int asf_read_marker(AVFormatContext *s, int64_t size)
{
    AVIOContext *pb = s->pb;
    ASFContext *asf = s->priv_data;
    int i, count, name_len, ret;
    char name[1024];

    avio_rl64(pb);            // reserved 16 bytes
    avio_rl64(pb);            // ...
    count = avio_rl32(pb);    // markers count
    avio_rl16(pb);            // reserved 2 bytes
    name_len = avio_rl16(pb); // name length
    avio_skip(pb, name_len);

    for (i = 0; i < count; i++) {
        int64_t pres_time;
        int name_len;

        if (avio_feof(pb))
            return AVERROR_INVALIDDATA;

        avio_rl64(pb);             // offset, 8 bytes
        pres_time = avio_rl64(pb); // presentation time
        pres_time -= asf->hdr.preroll * 10000;
        avio_rl16(pb);             // entry length
        avio_rl32(pb);             // send time
        avio_rl32(pb);             // flags
        name_len = avio_rl32(pb);  // name length
        if ((ret = avio_get_str16le(pb, name_len * 2, name,
                                    sizeof(name))) < name_len)
            avio_skip(pb, name_len - ret);
        avpriv_new_chapter(s, i, (AVRational) { 1, 10000000 }, pres_time,
                           AV_NOPTS_VALUE, name);
    }

    return 0;
}
