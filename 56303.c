null_hdr_print(netdissect_options *ndo, u_int family, u_int length)
{
	if (!ndo->ndo_qflag) {
		ND_PRINT((ndo, "AF %s (%u)",
			tok2str(bsd_af_values,"Unknown",family),family));
	} else {
		ND_PRINT((ndo, "%s",
			tok2str(bsd_af_values,"Unknown AF %u",family)));
	}

	ND_PRINT((ndo, ", length %u: ", length));
}
