static void get_str8(AVIOContext *pb, char *buf, int buf_size)
{
    get_strl(pb, buf, buf_size, avio_r8(pb));
}
