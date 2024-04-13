local int read_extra(unsigned len, int save)
{
    unsigned id, size, tmp2;
    unsigned long tmp4;

    /* process extra blocks */
    while (len >= 4) {
        id = GET2();
        size = GET2();
        if (g.in_eof)
            return -1;
        len -= 4;
        if (size > len)
            break;
        len -= size;
        if (id == 0x0001) {
            /* Zip64 Extended Information Extra Field */
            if (g.zip_ulen == LOW32 && size >= 8) {
                g.zip_ulen = GET4();
                SKIP(4);
                size -= 8;
            }
            if (g.zip_clen == LOW32 && size >= 8) {
                g.zip_clen = GET4();
                SKIP(4);
                size -= 8;
            }
        }
        if (save) {
            if ((id == 0x000d || id == 0x5855) && size >= 8) {
                /* PKWare Unix or Info-ZIP Type 1 Unix block */
                SKIP(4);
                g.stamp = tolong(GET4());
                size -= 8;
            }
            if (id == 0x5455 && size >= 5) {
                /* Extended Timestamp block */
                size--;
                if (GET() & 1) {
                    g.stamp = tolong(GET4());
                    size -= 4;
                }
            }
        }
        SKIP(size);
    }
    SKIP(len);
    return 0;
}
