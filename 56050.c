isis_print_ip_reach_subtlv(netdissect_options *ndo,
                           const uint8_t *tptr, int subt, int subl,
                           const char *ident)
{
    /* first lets see if we know the subTLVs name*/
    ND_PRINT((ndo, "%s%s subTLV #%u, length: %u",
              ident, tok2str(isis_ext_ip_reach_subtlv_values, "unknown", subt),
              subt, subl));

    ND_TCHECK2(*tptr,subl);

    switch(subt) {
    case ISIS_SUBTLV_EXTD_IP_REACH_MGMT_PREFIX_COLOR: /* fall through */
    case ISIS_SUBTLV_EXTD_IP_REACH_ADMIN_TAG32:
        while (subl >= 4) {
	    ND_PRINT((ndo, ", 0x%08x (=%u)",
		   EXTRACT_32BITS(tptr),
		   EXTRACT_32BITS(tptr)));
	    tptr+=4;
	    subl-=4;
	}
	break;
    case ISIS_SUBTLV_EXTD_IP_REACH_ADMIN_TAG64:
        while (subl >= 8) {
	    ND_PRINT((ndo, ", 0x%08x%08x",
		   EXTRACT_32BITS(tptr),
		   EXTRACT_32BITS(tptr+4)));
	    tptr+=8;
	    subl-=8;
	}
	break;
    default:
	if (!print_unknown_data(ndo, tptr, "\n\t\t    ", subl))
	  return(0);
	break;
    }
    return(1);

trunc:
    ND_PRINT((ndo, "%s", ident));
    ND_PRINT((ndo, "%s", tstr));
    return(0);
}
