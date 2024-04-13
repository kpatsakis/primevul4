local void unlzw(void)
{
    unsigned bits;              /* current bits per code (9..16) */
    unsigned mask;              /* mask for current bits codes = (1<<bits)-1 */
    bits_t buf;                 /* bit buffer (need 23 bits) */
    unsigned left;              /* bits left in buf (0..7 after code pulled) */
    off_t mark;                 /* offset where last change in bits began */
    unsigned code;              /* code, table traversal index */
    unsigned max;               /* maximum bits per code for this stream */
    unsigned flags;             /* compress flags, then block compress flag */
    unsigned end;               /* last valid entry in prefix/suffix tables */
    unsigned prev;              /* previous code */
    unsigned final;             /* last character written for previous code */
    unsigned stack;             /* next position for reversed string */
    unsigned outcnt;            /* bytes in output buffer */
    /* memory for unlzw() -- the first 256 entries of prefix[] and suffix[] are
       never used, so could have offset the index but it's faster to waste a
       little memory */
    uint_least16_t prefix[65536];       /* index to LZW prefix string */
    unsigned char suffix[65536];        /* one-character LZW suffix */
    unsigned char match[65280 + 2];     /* buffer for reversed match */

    /* process remainder of compress header -- a flags byte */
    g.out_tot = 0;
    if (NOMORE())
        bail("lzw premature end: ", g.inf);
    flags = NEXT();
    if (flags & 0x60)
        bail("unknown lzw flags set: ", g.inf);
    max = flags & 0x1f;
    if (max < 9 || max > 16)
        bail("lzw bits out of range: ", g.inf);
    if (max == 9)                           /* 9 doesn't really mean 9 */
        max = 10;
    flags &= 0x80;                          /* true if block compress */

    /* mark the start of the compressed data for computing the first flush */
    mark = g.in_tot - g.in_left;

    /* clear table, start at nine bits per symbol */
    bits = 9;
    mask = 0x1ff;
    end = flags ? 256 : 255;

    /* set up: get first 9-bit code, which is the first decompressed byte, but
       don't create a table entry until the next code */
    if (NOMORE())                           /* no compressed data is ok */
        return;
    buf = NEXT();
    if (NOMORE())
        bail("lzw premature end: ", g.inf); /* need at least nine bits */
    buf += NEXT() << 8;
    final = prev = buf & mask;              /* code */
    buf >>= bits;
    left = 16 - bits;
    if (prev > 255)
        bail("invalid lzw code: ", g.inf);
    out_buf[0] = final;                     /* write first decompressed byte */
    outcnt = 1;

    /* decode codes */
    stack = 0;
    for (;;) {
        /* if the table will be full after this, increment the code size */
        if (end >= mask && bits < max) {
            /* flush unused input bits and bytes to next 8*bits bit boundary
               (this is a vestigial aspect of the compressed data format
               derived from an implementation that made use of a special VAX
               machine instruction!) */
            {
                unsigned rem = ((g.in_tot - g.in_left) - mark) % bits;
                if (rem)
                    rem = bits - rem;
                while (rem > g.in_left) {
                    rem -= g.in_left;
                    if (load() == 0)
                        break;
                }
                g.in_left -= rem;
                g.in_next += rem;
            }
            buf = 0;
            left = 0;

            /* mark this new location for computing the next flush */
            mark = g.in_tot - g.in_left;

            /* go to the next number of bits per symbol */
            bits++;
            mask <<= 1;
            mask++;
        }

        /* get a code of bits bits */
        if (NOMORE())
            break;                          /* end of compressed data */
        buf += (bits_t)(NEXT()) << left;
        left += 8;
        if (left < bits) {
            if (NOMORE())
                bail("lzw premature end: ", g.inf);
            buf += (bits_t)(NEXT()) << left;
            left += 8;
        }
        code = buf & mask;
        buf >>= bits;
        left -= bits;

        /* process clear code (256) */
        if (code == 256 && flags) {
            /* flush unused input bits and bytes to next 8*bits bit boundary */
            {
                unsigned rem = ((g.in_tot - g.in_left) - mark) % bits;
                if (rem)
                    rem = bits - rem;
                while (rem > g.in_left) {
                    rem -= g.in_left;
                    if (load() == 0)
                        break;
                }
                g.in_left -= rem;
                g.in_next += rem;
            }
            buf = 0;
            left = 0;

            /* mark this new location for computing the next flush */
            mark = g.in_tot - g.in_left;

            /* go back to nine bits per symbol */
            bits = 9;                       /* initialize bits and mask */
            mask = 0x1ff;
            end = 255;                      /* empty table */
            continue;                       /* get next code */
        }

        /* special code to reuse last match */
        {
            unsigned temp = code;           /* save the current code */
            if (code > end) {
                /* Be picky on the allowed code here, and make sure that the
                   code we drop through (prev) will be a valid index so that
                   random input does not cause an exception. */
                if (code != end + 1 || prev > end)
                    bail("invalid lzw code: ", g.inf);
                match[stack++] = final;
                code = prev;
            }

            /* walk through linked list to generate output in reverse order */
            while (code >= 256) {
                match[stack++] = suffix[code];
                code = prefix[code];
            }
            match[stack++] = code;
            final = code;

            /* link new table entry */
            if (end < mask) {
                end++;
                prefix[end] = prev;
                suffix[end] = final;
            }

            /* set previous code for next iteration */
            prev = temp;
        }

        /* write output in forward order */
        while (stack > OUTSIZE - outcnt) {
            while (outcnt < OUTSIZE)
                out_buf[outcnt++] = match[--stack];
            g.out_tot += outcnt;
            if (g.decode == 1)
                writen(g.outd, out_buf, outcnt);
            outcnt = 0;
        }
        do {
            out_buf[outcnt++] = match[--stack];
        } while (stack);
    }

    /* write any remaining buffered output */
    g.out_tot += outcnt;
    if (outcnt && g.decode == 1)
        writen(g.outd, out_buf, outcnt);
}
