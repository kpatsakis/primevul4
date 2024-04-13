isis_print_ext_is_reach(netdissect_options *ndo,
                        const uint8_t *tptr, const char *ident, int tlv_type)
{
    char ident_buffer[20];
    int subtlv_type,subtlv_len,subtlv_sum_len;
    int proc_bytes = 0; /* how many bytes did we process ? */

    if (!ND_TTEST2(*tptr, NODE_ID_LEN))
        return(0);

    ND_PRINT((ndo, "%sIS Neighbor: %s", ident, isis_print_id(tptr, NODE_ID_LEN)));
    tptr+=(NODE_ID_LEN);

    if (tlv_type != ISIS_TLV_IS_ALIAS_ID) { /* the Alias TLV Metric field is implicit 0 */
        if (!ND_TTEST2(*tptr, 3))    /* and is therefore skipped */
	    return(0);
	ND_PRINT((ndo, ", Metric: %d", EXTRACT_24BITS(tptr)));
	tptr+=3;
    }

    if (!ND_TTEST2(*tptr, 1))
        return(0);
    subtlv_sum_len=*(tptr++); /* read out subTLV length */
    proc_bytes=NODE_ID_LEN+3+1;
    ND_PRINT((ndo, ", %ssub-TLVs present",subtlv_sum_len ? "" : "no "));
    if (subtlv_sum_len) {
        ND_PRINT((ndo, " (%u)", subtlv_sum_len));
        while (subtlv_sum_len>0) {
            if (!ND_TTEST2(*tptr,2))
                return(0);
            subtlv_type=*(tptr++);
            subtlv_len=*(tptr++);
            /* prepend the indent string */
            snprintf(ident_buffer, sizeof(ident_buffer), "%s  ",ident);
            if (!isis_print_is_reach_subtlv(ndo, tptr, subtlv_type, subtlv_len, ident_buffer))
                return(0);
            tptr+=subtlv_len;
            subtlv_sum_len-=(subtlv_len+2);
            proc_bytes+=(subtlv_len+2);
        }
    }
    return(proc_bytes);
}
