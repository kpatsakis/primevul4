bgp_print(netdissect_options *ndo,
          const u_char *dat, int length)
{
	const u_char *p;
	const u_char *ep;
	const u_char *start;
	const u_char marker[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	};
	struct bgp bgp;
	uint16_t hlen;

	ep = dat + length;
	if (ndo->ndo_snapend < dat + length)
		ep = ndo->ndo_snapend;

	ND_PRINT((ndo, ": BGP"));

        if (ndo->ndo_vflag < 1) /* lets be less chatty */
                return;

	p = dat;
	start = p;
	while (p < ep) {
		if (!ND_TTEST2(p[0], 1))
			break;
		if (p[0] != 0xff) {
			p++;
			continue;
		}

		if (!ND_TTEST2(p[0], sizeof(marker)))
			break;
		if (memcmp(p, marker, sizeof(marker)) != 0) {
			p++;
			continue;
		}

		/* found BGP header */
		ND_TCHECK2(p[0], BGP_SIZE);	/*XXX*/
		memcpy(&bgp, p, BGP_SIZE);

		if (start != p)
			ND_PRINT((ndo, " [|BGP]"));

		hlen = ntohs(bgp.bgp_len);
		if (hlen < BGP_SIZE) {
			ND_PRINT((ndo, "\n[|BGP Bogus header length %u < %u]", hlen,
			    BGP_SIZE));
			break;
		}

		if (ND_TTEST2(p[0], hlen)) {
			if (!bgp_header_print(ndo, p, hlen))
				return;
			p += hlen;
			start = p;
		} else {
			ND_PRINT((ndo, "\n[|BGP %s]",
			       tok2str(bgp_msg_values,
					  "Unknown Message Type",
					  bgp.bgp_type)));
			break;
		}
	}

	return;

trunc:
	ND_PRINT((ndo, " [|BGP]"));
}
