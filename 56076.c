bgp_vpn_rd_print(netdissect_options *ndo,
                 const u_char *pptr)
{
   /* allocate space for the largest possible string */
    static char rd[sizeof("xxxxxxxxxx:xxxxx (xxx.xxx.xxx.xxx:xxxxx)")];
    char *pos = rd;

    /* ok lets load the RD format */
    switch (EXTRACT_16BITS(pptr)) {

        /* 2-byte-AS:number fmt*/
    case 0:
        snprintf(pos, sizeof(rd) - (pos - rd), "%u:%u (= %u.%u.%u.%u)",
                 EXTRACT_16BITS(pptr+2),
                 EXTRACT_32BITS(pptr+4),
                 *(pptr+4), *(pptr+5), *(pptr+6), *(pptr+7));
        break;
        /* IP-address:AS fmt*/

    case 1:
        snprintf(pos, sizeof(rd) - (pos - rd), "%u.%u.%u.%u:%u",
            *(pptr+2), *(pptr+3), *(pptr+4), *(pptr+5), EXTRACT_16BITS(pptr+6));
        break;

        /* 4-byte-AS:number fmt*/
    case 2:
	snprintf(pos, sizeof(rd) - (pos - rd), "%s:%u (%u.%u.%u.%u:%u)",
	    as_printf(ndo, astostr, sizeof(astostr), EXTRACT_32BITS(pptr+2)),
	    EXTRACT_16BITS(pptr+6), *(pptr+2), *(pptr+3), *(pptr+4),
	    *(pptr+5), EXTRACT_16BITS(pptr+6));
        break;
    default:
        snprintf(pos, sizeof(rd) - (pos - rd), "unknown RD format");
        break;
    }
    pos += strlen(pos);
    *(pos) = '\0';
    return (rd);
}
