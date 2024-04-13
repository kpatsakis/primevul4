local void single_compress(int reset)
{
    size_t got;                     /* amount of data in in[] */
    size_t more;                    /* amount of data in next[] (0 if eof) */
    size_t start;                   /* start of data in next[] */
    size_t have;                    /* bytes in current block for -i */
    size_t hist;                    /* offset of permitted history */
    int fresh;                      /* if true, reset compression history */
    unsigned hash;                  /* hash for rsyncable */
    unsigned char *scan;            /* pointer for hash computation */
    size_t left;                    /* bytes left to compress after hash hit */
    unsigned long head;             /* header length */
    unsigned long ulen;             /* total uncompressed size (overflow ok) */
    unsigned long clen;             /* total compressed size (overflow ok) */
    unsigned long check;            /* check value of uncompressed data */
    static unsigned out_size;       /* size of output buffer */
    static unsigned char *in, *next, *out;  /* reused i/o buffers */
    static z_stream *strm = NULL;   /* reused deflate structure */

    /* if requested, just release the allocations and return */
    if (reset) {
        if (strm != NULL) {
            (void)deflateEnd(strm);
            FREE(strm);
            FREE(out);
            FREE(next);
            FREE(in);
            strm = NULL;
        }
        return;
    }

    /* initialize the deflate structure if this is the first time */
    if (strm == NULL) {
        out_size = g.block > MAXP2 ? MAXP2 : (unsigned)g.block;
        if ((in = MALLOC(g.block + DICT)) == NULL ||
            (next = MALLOC(g.block + DICT)) == NULL ||
            (out = MALLOC(out_size)) == NULL ||
            (strm = MALLOC(sizeof(z_stream))) == NULL)
            bail("not enough memory", "");
        strm->zfree = ZFREE;
        strm->zalloc = ZALLOC;
        strm->opaque = OPAQUE;
        if (deflateInit2(strm, 6, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) !=
                         Z_OK)
            bail("not enough memory", "");
    }

    /* write header */
    head = put_header();

    /* set compression level in case it changed */
    if (g.level <= 9) {
        (void)deflateReset(strm);
        (void)deflateParams(strm, g.level, Z_DEFAULT_STRATEGY);
    }

    /* do raw deflate and calculate check value */
    got = 0;
    more = readn(g.ind, next, g.block);
    ulen = (unsigned long)more;
    start = 0;
    hist = 0;
    clen = 0;
    have = 0;
    check = CHECK(0L, Z_NULL, 0);
    hash = RSYNCHIT;
    do {
        /* get data to compress, see if there is any more input */
        if (got == 0) {
            scan = in;  in = next;  next = scan;
            strm->next_in = in + start;
            got = more;
            if (g.level > 9) {
                left = start + more - hist;
                if (left > DICT)
                    left = DICT;
                memcpy(next, in + ((start + more) - left), left);
                start = left;
                hist = 0;
            }
            else
                start = 0;
            more = readn(g.ind, next + start, g.block);
            ulen += (unsigned long)more;
        }

        /* if rsyncable, compute hash until a hit or the end of the block */
        left = 0;
        if (g.rsync && got) {
            scan = strm->next_in;
            left = got;
            do {
                if (left == 0) {
                    /* went to the end -- if no more or no hit in size bytes,
                       then proceed to do a flush or finish with got bytes */
                    if (more == 0 || got == g.block)
                        break;

                    /* fill in[] with what's left there and as much as possible
                       from next[] -- set up to continue hash hit search */
                    if (g.level > 9) {
                        left = (strm->next_in - in) - hist;
                        if (left > DICT)
                            left = DICT;
                    }
                    memmove(in, strm->next_in - left, left + got);
                    hist = 0;
                    strm->next_in = in + left;
                    scan = in + left + got;
                    left = more > g.block - got ? g.block - got : more;
                    memcpy(scan, next + start, left);
                    got += left;
                    more -= left;
                    start += left;

                    /* if that emptied the next buffer, try to refill it */
                    if (more == 0) {
                        more = readn(g.ind, next, g.block);
                        ulen += (unsigned long)more;
                        start = 0;
                    }
                }
                left--;
                hash = ((hash << 1) ^ *scan++) & RSYNCMASK;
            } while (hash != RSYNCHIT);
            got -= left;
        }

        /* clear history for --independent option */
        fresh = 0;
        if (!g.setdict) {
            have += got;
            if (have > g.block) {
                fresh = 1;
                have = got;
            }
        }

        if (g.level <= 9) {
            /* clear history if requested */
            if (fresh)
                (void)deflateReset(strm);

            /* compress MAXP2-size chunks in case unsigned type is small */
            while (got > MAXP2) {
                strm->avail_in = MAXP2;
                check = CHECK(check, strm->next_in, strm->avail_in);
                DEFLATE_WRITE(Z_NO_FLUSH);
                got -= MAXP2;
            }

            /* compress the remainder, emit a block, finish if end of input */
            strm->avail_in = (unsigned)got;
            got = left;
            check = CHECK(check, strm->next_in, strm->avail_in);
            if (more || got) {
#if ZLIB_VERNUM >= 0x1260
                int bits;

                DEFLATE_WRITE(Z_BLOCK);
                (void)deflatePending(strm, Z_NULL, &bits);
                if (bits & 1)
                    DEFLATE_WRITE(Z_SYNC_FLUSH);
                else if (bits & 7) {
                    do {
                        bits = deflatePrime(strm, 10, 2);
                        assert(bits == Z_OK);
                        (void)deflatePending(strm, Z_NULL, &bits);
                    } while (bits & 7);
                    DEFLATE_WRITE(Z_NO_FLUSH);
                }
#else
                DEFLATE_WRITE(Z_SYNC_FLUSH);
#endif
            }
            else
                DEFLATE_WRITE(Z_FINISH);
        }
        else {
            /* compress got bytes using zopfli, bring to byte boundary */
            unsigned char bits, *out;
            size_t outsize, off;

            /* discard history if requested */
            off = strm->next_in - in;
            if (fresh)
                hist = off;

            out = NULL;
            outsize = 0;
            bits = 0;
            ZopfliDeflatePart(&g.zopts, 2, !(more || left),
                              in + hist, off - hist, (off - hist) + got,
                              &bits, &out, &outsize);
            bits &= 7;
            if ((more || left) && bits) {
                if (bits & 1) {
                    writen(g.outd, out, outsize);
                    if (bits == 7)
                        writen(g.outd, (unsigned char *)"\0", 1);
                    writen(g.outd, (unsigned char *)"\0\0\xff\xff", 4);
                }
                else {
                    assert(outsize > 0);
                    writen(g.outd, out, outsize - 1);
                    do {
                        out[outsize - 1] += 2 << bits;
                        writen(g.outd, out + outsize - 1, 1);
                        out[outsize - 1] = 0;
                        bits += 2;
                    } while (bits < 8);
                    writen(g.outd, out + outsize - 1, 1);
                }
            }
            else
                writen(g.outd, out, outsize);
            free(out);
            while (got > MAXP2) {
                check = CHECK(check, strm->next_in, MAXP2);
                strm->next_in += MAXP2;
                got -= MAXP2;
            }
            check = CHECK(check, strm->next_in, (unsigned)got);
            strm->next_in += got;
            got = left;
        }

        /* do until no more input */
    } while (more || got);

    /* write trailer */
    put_trailer(ulen, clen, check, head);
}
