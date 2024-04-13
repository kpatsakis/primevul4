ppp_bsdos_if_print(netdissect_options *ndo _U_,
                   const struct pcap_pkthdr *h _U_, register const u_char *p _U_)
{
	register int hdrlength;
#ifdef __bsdi__
	register u_int length = h->len;
	register u_int caplen = h->caplen;
	uint16_t ptype;
	const u_char *q;
	int i;

	if (caplen < PPP_BSDI_HDRLEN) {
		ND_PRINT((ndo, "[|ppp]"));
		return (caplen)
	}

	hdrlength = 0;

#if 0
	if (p[0] == PPP_ADDRESS && p[1] == PPP_CONTROL) {
		if (ndo->ndo_eflag)
			ND_PRINT((ndo, "%02x %02x ", p[0], p[1]));
		p += 2;
		hdrlength = 2;
	}

	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "%d ", length));
	/* Retrieve the protocol type */
	if (*p & 01) {
		/* Compressed protocol field */
		ptype = *p;
		if (ndo->ndo_eflag)
			ND_PRINT((ndo, "%02x ", ptype));
		p++;
		hdrlength += 1;
	} else {
		/* Un-compressed protocol field */
		ptype = EXTRACT_16BITS(p);
		if (ndo->ndo_eflag)
			ND_PRINT((ndo, "%04x ", ptype));
		p += 2;
		hdrlength += 2;
	}
#else
	ptype = 0;	/*XXX*/
	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "%c ", p[SLC_DIR] ? 'O' : 'I'));
	if (p[SLC_LLHL]) {
		/* link level header */
		struct ppp_header *ph;

		q = p + SLC_BPFHDRLEN;
		ph = (struct ppp_header *)q;
		if (ph->phdr_addr == PPP_ADDRESS
		 && ph->phdr_ctl == PPP_CONTROL) {
			if (ndo->ndo_eflag)
				ND_PRINT((ndo, "%02x %02x ", q[0], q[1]));
			ptype = EXTRACT_16BITS(&ph->phdr_type);
			if (ndo->ndo_eflag && (ptype == PPP_VJC || ptype == PPP_VJNC)) {
				ND_PRINT((ndo, "%s ", tok2str(ppptype2str,
						"proto-#%d", ptype)));
			}
		} else {
			if (ndo->ndo_eflag) {
				ND_PRINT((ndo, "LLH=["));
				for (i = 0; i < p[SLC_LLHL]; i++)
					ND_PRINT((ndo, "%02x", q[i]));
				ND_PRINT((ndo, "] "));
			}
		}
	}
	if (ndo->ndo_eflag)
		ND_PRINT((ndo, "%d ", length));
	if (p[SLC_CHL]) {
		q = p + SLC_BPFHDRLEN + p[SLC_LLHL];

		switch (ptype) {
		case PPP_VJC:
			ptype = vjc_print(ndo, q, ptype);
			hdrlength = PPP_BSDI_HDRLEN;
			p += hdrlength;
			switch (ptype) {
			case PPP_IP:
				ip_print(ndo, p, length);
				break;
			case PPP_IPV6:
				ip6_print(ndo, p, length);
				break;
			case PPP_MPLS_UCAST:
			case PPP_MPLS_MCAST:
				mpls_print(ndo, p, length);
				break;
			}
			goto printx;
		case PPP_VJNC:
			ptype = vjc_print(ndo, q, ptype);
			hdrlength = PPP_BSDI_HDRLEN;
			p += hdrlength;
			switch (ptype) {
			case PPP_IP:
				ip_print(ndo, p, length);
				break;
			case PPP_IPV6:
				ip6_print(ndo, p, length);
				break;
			case PPP_MPLS_UCAST:
			case PPP_MPLS_MCAST:
				mpls_print(ndo, p, length);
				break;
			}
			goto printx;
		default:
			if (ndo->ndo_eflag) {
				ND_PRINT((ndo, "CH=["));
				for (i = 0; i < p[SLC_LLHL]; i++)
					ND_PRINT((ndo, "%02x", q[i]));
				ND_PRINT((ndo, "] "));
			}
			break;
		}
	}

	hdrlength = PPP_BSDI_HDRLEN;
#endif

	length -= hdrlength;
	p += hdrlength;

	switch (ptype) {
	case PPP_IP:
		ip_print(p, length);
		break;
	case PPP_IPV6:
		ip6_print(ndo, p, length);
		break;
	case PPP_MPLS_UCAST:
	case PPP_MPLS_MCAST:
		mpls_print(ndo, p, length);
		break;
	default:
		ND_PRINT((ndo, "%s ", tok2str(ppptype2str, "unknown PPP protocol (0x%04x)", ptype)));
	}

printx:
#else /* __bsdi */
	hdrlength = 0;
#endif /* __bsdi__ */
	return (hdrlength);
}
