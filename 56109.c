format_nid(const u_char *data)
{
    static char buf[4][sizeof("01:01:01:01")];
    static int i = 0;
    i = (i + 1) % 4;
    snprintf(buf[i], sizeof(buf[i]), "%02x:%02x:%02x:%02x",
             data[0], data[1], data[2], data[3]);
    return buf[i];
}
