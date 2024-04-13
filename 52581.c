asn1length(unsigned char **astream)
{
    int length;         /* resulting length */
    int sublen;         /* sublengths */
    int blen;           /* bytes of length */
    unsigned char *p;   /* substring searching */

    if (**astream & 0x80) {
        blen = **astream & 0x7f;
        if (blen > 3) {
            return(-1);
        }
        for (++*astream, length = 0; blen; ++*astream, blen--) {
            length = (length << 8) | **astream;
        }
        if (length == 0) {
            /* indefinite length, figure out by hand */
            p = *astream;
            p++;
            while (1) {
                /* compute value length. */
                if ((sublen = asn1length(&p)) < 0) {
                    return(-1);
                }
                p += sublen;
                /* check for termination */
                if ((!*p++) && (!*p)) {
                    p++;
                    break;
                }
            }
            length = p - *astream;
        }
    } else {
        length = **astream;
        ++*astream;
    }
    return(length);
}
