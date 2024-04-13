static int get_num(AVIOContext *pb, int *len)
{
    int n, n1;

    n = avio_rb16(pb);
    (*len)-=2;
    n &= 0x7FFF;
    if (n >= 0x4000) {
        return n - 0x4000;
    } else {
        n1 = avio_rb16(pb);
        (*len)-=2;
        return (n << 16) | n1;
    }
}
