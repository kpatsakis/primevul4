fr_if_print(netdissect_options *ndo,
            const struct pcap_pkthdr *h, register const u_char *p)
{
	register u_int length = h->len;
	register u_int caplen = h->caplen;

        ND_TCHECK2(*p, 4); /* minimum frame header length */

        if ((length = fr_print(ndo, p, length)) == 0)
            return (0);
        else
            return length;
 trunc:
        ND_PRINT((ndo, "[|fr]"));
        return caplen;
}
