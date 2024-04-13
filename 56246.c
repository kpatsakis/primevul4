juniper_chdlc_print(netdissect_options *ndo,
                    const struct pcap_pkthdr *h, register const u_char *p)
{
        struct juniper_l2info_t l2info;

        l2info.pictype = DLT_JUNIPER_CHDLC;
        if (juniper_parse_header(ndo, p, h, &l2info) == 0)
            return l2info.header_len;

        p+=l2info.header_len;
        /* this DLT contains nothing but raw c-hdlc frames */
        chdlc_print(ndo, p, l2info.length);
        return l2info.header_len;
}
