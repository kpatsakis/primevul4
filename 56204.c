pktap_header_print(netdissect_options *ndo, const u_char *bp, u_int length)
{
	const pktap_header_t *hdr;
	uint32_t dlt, hdrlen;
	const char *dltname;

	hdr = (const pktap_header_t *)bp;

	dlt = EXTRACT_LE_32BITS(&hdr->pkt_dlt);
	hdrlen = EXTRACT_LE_32BITS(&hdr->pkt_len);
	dltname = pcap_datalink_val_to_name(dlt);
	if (!ndo->ndo_qflag) {
		ND_PRINT((ndo,"DLT %s (%d) len %d",
			  (dltname != NULL ? dltname : "UNKNOWN"), dlt, hdrlen));
        } else {
		ND_PRINT((ndo,"%s", (dltname != NULL ? dltname : "UNKNOWN")));
        }

	ND_PRINT((ndo, ", length %u: ", length));
}
