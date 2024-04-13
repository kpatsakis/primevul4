bgp_open_print(netdissect_options *ndo,
               const u_char *dat, int length)
{
	struct bgp_open bgpo;
	struct bgp_opt bgpopt;
	const u_char *opt;
	int i;

	ND_TCHECK2(dat[0], BGP_OPEN_SIZE);
	memcpy(&bgpo, dat, BGP_OPEN_SIZE);

	ND_PRINT((ndo, "\n\t  Version %d, ", bgpo.bgpo_version));
	ND_PRINT((ndo, "my AS %s, ",
	    as_printf(ndo, astostr, sizeof(astostr), ntohs(bgpo.bgpo_myas))));
	ND_PRINT((ndo, "Holdtime %us, ", ntohs(bgpo.bgpo_holdtime)));
	ND_PRINT((ndo, "ID %s", ipaddr_string(ndo, &bgpo.bgpo_id)));
	ND_PRINT((ndo, "\n\t  Optional parameters, length: %u", bgpo.bgpo_optlen));

        /* some little sanity checking */
        if (length < bgpo.bgpo_optlen+BGP_OPEN_SIZE)
            return;

	/* ugly! */
	opt = &((const struct bgp_open *)dat)->bgpo_optlen;
	opt++;

	i = 0;
	while (i < bgpo.bgpo_optlen) {
		ND_TCHECK2(opt[i], BGP_OPT_SIZE);
		memcpy(&bgpopt, &opt[i], BGP_OPT_SIZE);
		if (i + 2 + bgpopt.bgpopt_len > bgpo.bgpo_optlen) {
			ND_PRINT((ndo, "\n\t     Option %d, length: %u", bgpopt.bgpopt_type, bgpopt.bgpopt_len));
			break;
		}

		ND_PRINT((ndo, "\n\t    Option %s (%u), length: %u",
		       tok2str(bgp_opt_values,"Unknown",
				  bgpopt.bgpopt_type),
		       bgpopt.bgpopt_type,
		       bgpopt.bgpopt_len));

		/* now let's decode the options we know*/
		switch(bgpopt.bgpopt_type) {

		case BGP_OPT_CAP:
			bgp_capabilities_print(ndo, &opt[i+BGP_OPT_SIZE],
			    bgpopt.bgpopt_len);
			break;

		case BGP_OPT_AUTH:
		default:
		       ND_PRINT((ndo, "\n\t      no decoder for option %u",
			   bgpopt.bgpopt_type));
		       break;
		}
		i += BGP_OPT_SIZE + bgpopt.bgpopt_len;
	}
	return;
trunc:
	ND_PRINT((ndo, "[|BGP]"));
}
