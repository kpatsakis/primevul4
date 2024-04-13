local void put_trailer(unsigned long ulen, unsigned long clen,
                       unsigned long check, unsigned long head)
{
    unsigned char tail[46];

    if (g.form > 1) {               /* zip */
        unsigned long cent;

        /* write data descriptor (as promised in local header) */
        PUT4L(tail, 0x08074b50UL);
        PUT4L(tail + 4, check);
        PUT4L(tail + 8, clen);
        PUT4L(tail + 12, ulen);
        writen(g.outd, tail, 16);

        /* write central file header */
        PUT4L(tail, 0x02014b50UL);  /* central header signature */
        tail[4] = 63;               /* obeyed version 6.3 of the zip spec */
        tail[5] = 255;              /* ignore external attributes */
        PUT2L(tail + 6, 20);        /* version needed to extract (2.0) */
        PUT2L(tail + 8, 8);         /* data descriptor is present */
        PUT2L(tail + 10, 8);        /* deflate */
        PUT4L(tail + 12, time2dos(g.mtime));
        PUT4L(tail + 16, check);    /* crc */
        PUT4L(tail + 20, clen);     /* compressed length */
        PUT4L(tail + 24, ulen);     /* uncompressed length */
        PUT2L(tail + 28, g.name == NULL ? 1 :   /* length of name */
                                          strlen(g.name));
        PUT2L(tail + 30, 9);        /* length of extra field (see below) */
        PUT2L(tail + 32, 0);        /* no file comment */
        PUT2L(tail + 34, 0);        /* disk number 0 */
        PUT2L(tail + 36, 0);        /* internal file attributes */
        PUT4L(tail + 38, 0);        /* external file attributes (ignored) */
        PUT4L(tail + 42, 0);        /* offset of local header */
        writen(g.outd, tail, 46);   /* write central file header */
        cent = 46;

        /* write file name (use "-" for stdin) */
        if (g.name == NULL)
            writen(g.outd, (unsigned char *)"-", 1);
        else
            writen(g.outd, (unsigned char *)g.name, strlen(g.name));
        cent += g.name == NULL ? 1 : strlen(g.name);

        /* write extended timestamp extra field block (9 bytes) */
        PUT2L(tail, 0x5455);        /* extended timestamp signature */
        PUT2L(tail + 2, 5);         /* number of data bytes in this block */
        tail[4] = 1;                /* flag presence of mod time */
        PUT4L(tail + 5, g.mtime);   /* mod time */
        writen(g.outd, tail, 9);    /* write extra field block */
        cent += 9;

        /* write end of central directory record */
        PUT4L(tail, 0x06054b50UL);  /* end of central directory signature */
        PUT2L(tail + 4, 0);         /* number of this disk */
        PUT2L(tail + 6, 0);         /* disk with start of central directory */
        PUT2L(tail + 8, 1);         /* number of entries on this disk */
        PUT2L(tail + 10, 1);        /* total number of entries */
        PUT4L(tail + 12, cent);     /* size of central directory */
        PUT4L(tail + 16, head + clen + 16); /* offset of central directory */
        PUT2L(tail + 20, 0);        /* no zip file comment */
        writen(g.outd, tail, 22);   /* write end of central directory record */
    }
    else if (g.form) {              /* zlib */
        PUT4M(tail, check);
        writen(g.outd, tail, 4);
    }
    else {                          /* gzip */
        PUT4L(tail, check);
        PUT4L(tail + 4, ulen);
        writen(g.outd, tail, 8);
    }
}
