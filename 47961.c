local void list_info(void)
{
    int method;             /* get_header() return value */
    size_t n;               /* available trailer bytes */
    off_t at;               /* used to calculate compressed length */
    unsigned char tail[8];  /* trailer containing check and length */
    unsigned long check, len;   /* check value and length from trailer */

    /* initialize input buffer */
    in_init();

    /* read header information and position input after header */
    method = get_header(1);
    if (method < 0) {
        RELEASE(g.hname);
        if (method != -1 && g.verbosity > 1)
            complain("%s not a compressed file -- skipping", g.inf);
        return;
    }

    /* list zip file */
    if (g.form > 1) {
        g.in_tot = g.zip_clen;
        show_info(method, g.zip_crc, g.zip_ulen, 0);
        return;
    }

    /* list zlib file */
    if (g.form == 1) {
        at = lseek(g.ind, 0, SEEK_END);
        if (at == -1) {
            check = 0;
            do {
                len = g.in_left < 4 ? g.in_left : 4;
                g.in_next += g.in_left - len;
                while (len--)
                    check = (check << 8) + *g.in_next++;
            } while (load() != 0);
            check &= LOW32;
        }
        else {
            g.in_tot = at;
            lseek(g.ind, -4, SEEK_END);
            readn(g.ind, tail, 4);
            check = PULL4M(tail);
        }
        g.in_tot -= 6;
        show_info(method, check, 0, 0);
        return;
    }

    /* list lzw file */
    if (method == 257) {
        at = lseek(g.ind, 0, SEEK_END);
        if (at == -1)
            while (load() != 0)
                ;
        else
            g.in_tot = at;
        g.in_tot -= 3;
        show_info(method, 0, 0, 0);
        return;
    }

    /* skip to end to get trailer (8 bytes), compute compressed length */
    if (g.in_short) {                   /* whole thing already read */
        if (g.in_left < 8) {
            complain("%s not a valid gzip file -- skipping", g.inf);
            return;
        }
        g.in_tot = g.in_left - 8;       /* compressed size */
        memcpy(tail, g.in_next + (g.in_left - 8), 8);
    }
    else if ((at = lseek(g.ind, -8, SEEK_END)) != -1) {
        g.in_tot = at - g.in_tot + g.in_left;   /* compressed size */
        readn(g.ind, tail, 8);          /* get trailer */
    }
    else {                              /* can't seek */
        at = g.in_tot - g.in_left;      /* save header size */
        do {
            n = g.in_left < 8 ? g.in_left : 8;
            memcpy(tail, g.in_next + (g.in_left - n), n);
            load();
        } while (g.in_left == BUF);     /* read until end */
        if (g.in_left < 8) {
            if (n + g.in_left < 8) {
                complain("%s not a valid gzip file -- skipping", g.inf);
                return;
            }
            if (g.in_left) {
                if (n + g.in_left > 8)
                    memcpy(tail, tail + n - (8 - g.in_left), 8 - g.in_left);
                memcpy(tail + 8 - g.in_left, g.in_next, g.in_left);
            }
        }
        else
            memcpy(tail, g.in_next + (g.in_left - 8), 8);
        g.in_tot -= at + 8;
    }
    if (g.in_tot < 2) {
        complain("%s not a valid gzip file -- skipping", g.inf);
        return;
    }

    /* convert trailer to check and uncompressed length (modulo 2^32) */
    check = PULL4L(tail);
    len = PULL4L(tail + 4);

    /* list information about contents */
    show_info(method, check, len, 0);
    RELEASE(g.hname);
}
