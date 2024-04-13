local unsigned long put_header(void)
{
    unsigned long len;
    unsigned char head[30];

    if (g.form > 1) {               /* zip */
        /* write local header */
        PUT4L(head, 0x04034b50UL);  /* local header signature */
        PUT2L(head + 4, 20);        /* version needed to extract (2.0) */
        PUT2L(head + 6, 8);         /* flags: data descriptor follows data */
        PUT2L(head + 8, 8);         /* deflate */
        PUT4L(head + 10, time2dos(g.mtime));
        PUT4L(head + 14, 0);        /* crc (not here) */
        PUT4L(head + 18, 0);        /* compressed length (not here) */
        PUT4L(head + 22, 0);        /* uncompressed length (not here) */
        PUT2L(head + 26, g.name == NULL ? 1 :   /* length of name */
                                          strlen(g.name));
        PUT2L(head + 28, 9);        /* length of extra field (see below) */
        writen(g.outd, head, 30);   /* write local header */
        len = 30;

        /* write file name (use "-" for stdin) */
        if (g.name == NULL)
            writen(g.outd, (unsigned char *)"-", 1);
        else
            writen(g.outd, (unsigned char *)g.name, strlen(g.name));
        len += g.name == NULL ? 1 : strlen(g.name);

        /* write extended timestamp extra field block (9 bytes) */
        PUT2L(head, 0x5455);        /* extended timestamp signature */
        PUT2L(head + 2, 5);         /* number of data bytes in this block */
        head[4] = 1;                /* flag presence of mod time */
        PUT4L(head + 5, g.mtime);   /* mod time */
        writen(g.outd, head, 9);    /* write extra field block */
        len += 9;
    }
    else if (g.form) {              /* zlib */
        head[0] = 0x78;             /* deflate, 32K window */
        head[1] = (g.level >= 9 ? 3 :
                   (g.level == 1 ? 0 :
                    (g.level >= 6 || g.level == Z_DEFAULT_COMPRESSION ?
                        1 : 2))) << 6;
        head[1] += 31 - (((head[0] << 8) + head[1]) % 31);
        writen(g.outd, head, 2);
        len = 2;
    }
    else {                          /* gzip */
        head[0] = 31;
        head[1] = 139;
        head[2] = 8;                /* deflate */
        head[3] = g.name != NULL ? 8 : 0;
        PUT4L(head + 4, g.mtime);
        head[8] = g.level >= 9 ? 2 : (g.level == 1 ? 4 : 0);
        head[9] = 3;                /* unix */
        writen(g.outd, head, 10);
        len = 10;
        if (g.name != NULL)
            writen(g.outd, (unsigned char *)g.name, strlen(g.name) + 1);
        if (g.name != NULL)
            len += strlen(g.name) + 1;
    }
    return len;
}
