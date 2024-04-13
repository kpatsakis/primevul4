fetch_asn1_field(unsigned char *astream, unsigned int level,
                 unsigned int field, krb5_data *data)
{
    unsigned char *estream;     /* end of stream */
    int classes;                /* # classes seen so far this level */
    unsigned int levels = 0;            /* levels seen so far */
    int lastlevel = 1000;       /* last level seen */
    int length;                 /* various lengths */
    int tag;                    /* tag number */
    unsigned char savelen;      /* saved length of our field */

    classes = -1;
    /* we assume that the first identifier/length will tell us
       how long the entire stream is. */
    astream++;
    estream = astream;
    if ((length = asn1length(&astream)) < 0) {
        return(-1);
    }
    estream += length;
    /* search down the stream, checking identifiers.  we process identifiers
       until we hit the "level" we want, and then process that level for our
       subfield, always making sure we don't go off the end of the stream.  */
    while (astream < estream) {
        if (!asn1_id_constructed(*astream)) {
            return(-1);
        }
        if (asn1_id_class(*astream) == ASN1_CLASS_CTX) {
            if ((tag = (int)asn1_id_tag(*astream)) <= lastlevel) {
                levels++;
                classes = -1;
            }
            lastlevel = tag;
            if (levels == level) {
                /* in our context-dependent class, is this the one we're looking for ? */
                if (tag == (int)field) {
                    /* return length and data */
                    astream++;
                    savelen = *astream;
                    if ((length = asn1length(&astream)) < 0) {
                        return(-1);
                    }
                    data->length = length;
                    /* if the field length is indefinite, we will have to subtract two
                       (terminating octets) from the length returned since we don't want
                       to pass any info from the "wrapper" back.  asn1length will always return
                       the *total* length of the field, not just what's contained in it */
                    if ((savelen & 0xff) == 0x80) {
                        data->length -=2 ;
                    }
                    data->data = (char *)astream;
                    return(0);
                } else if (tag <= classes) {
                    /* we've seen this class before, something must be wrong */
                    return(-1);
                } else {
                    classes = tag;
                }
            }
        }
        /* if we're not on our level yet, process this value.  otherwise skip over it */
        astream++;
        if ((length = asn1length(&astream)) < 0) {
            return(-1);
        }
        if (levels == level) {
            astream += length;
        }
    }
    return(-1);
}
