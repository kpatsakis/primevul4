ns_rprint(netdissect_options *ndo,
          register const u_char *cp, register const u_char *bp, int is_mdns)
{
	register u_int i, class, opt_flags = 0;
	register u_short typ, len;
	register const u_char *rp;

	if (ndo->ndo_vflag) {
		ND_PRINT((ndo, " "));
		if ((cp = ns_nprint(ndo, cp, bp)) == NULL)
			return NULL;
	} else
		cp = ns_nskip(ndo, cp);

	if (cp == NULL || !ND_TTEST2(*cp, 10))
		return (ndo->ndo_snapend);

	/* print the type/qtype */
	typ = EXTRACT_16BITS(cp);
	cp += 2;
	/* print the class (if it's not IN and the type isn't OPT) */
	i = EXTRACT_16BITS(cp);
	cp += 2;
	if (is_mdns)
		class = (i & ~C_CACHE_FLUSH);
	else
		class = i;
	if (class != C_IN && typ != T_OPT)
		ND_PRINT((ndo, " %s", tok2str(ns_class2str, "(Class %d)", class)));
	if (is_mdns) {
		if (i & C_CACHE_FLUSH)
			ND_PRINT((ndo, " (Cache flush)"));
	}

	if (typ == T_OPT) {
		/* get opt flags */
		cp += 2;
		opt_flags = EXTRACT_16BITS(cp);
		/* ignore rest of ttl field */
		cp += 2;
	} else if (ndo->ndo_vflag > 2) {
		/* print ttl */
		ND_PRINT((ndo, " ["));
		unsigned_relts_print(ndo, EXTRACT_32BITS(cp));
		ND_PRINT((ndo, "]"));
		cp += 4;
	} else {
		/* ignore ttl */
		cp += 4;
	}

	len = EXTRACT_16BITS(cp);
	cp += 2;

	rp = cp + len;

	ND_PRINT((ndo, " %s", tok2str(ns_type2str, "Type%d", typ)));
	if (rp > ndo->ndo_snapend)
		return(NULL);

	switch (typ) {
	case T_A:
		if (!ND_TTEST2(*cp, sizeof(struct in_addr)))
			return(NULL);
		ND_PRINT((ndo, " %s", intoa(htonl(EXTRACT_32BITS(cp)))));
		break;

	case T_NS:
	case T_CNAME:
	case T_PTR:
#ifdef T_DNAME
	case T_DNAME:
#endif
		ND_PRINT((ndo, " "));
		if (ns_nprint(ndo, cp, bp) == NULL)
			return(NULL);
		break;

	case T_SOA:
		if (!ndo->ndo_vflag)
			break;
		ND_PRINT((ndo, " "));
		if ((cp = ns_nprint(ndo, cp, bp)) == NULL)
			return(NULL);
		ND_PRINT((ndo, " "));
		if ((cp = ns_nprint(ndo, cp, bp)) == NULL)
			return(NULL);
		if (!ND_TTEST2(*cp, 5 * 4))
			return(NULL);
		ND_PRINT((ndo, " %u", EXTRACT_32BITS(cp)));
		cp += 4;
		ND_PRINT((ndo, " %u", EXTRACT_32BITS(cp)));
		cp += 4;
		ND_PRINT((ndo, " %u", EXTRACT_32BITS(cp)));
		cp += 4;
		ND_PRINT((ndo, " %u", EXTRACT_32BITS(cp)));
		cp += 4;
		ND_PRINT((ndo, " %u", EXTRACT_32BITS(cp)));
		cp += 4;
		break;
	case T_MX:
		ND_PRINT((ndo, " "));
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		if (ns_nprint(ndo, cp + 2, bp) == NULL)
			return(NULL);
		ND_PRINT((ndo, " %d", EXTRACT_16BITS(cp)));
		break;

	case T_TXT:
		while (cp < rp) {
			ND_PRINT((ndo, " \""));
			cp = ns_cprint(ndo, cp);
			if (cp == NULL)
				return(NULL);
			ND_PRINT((ndo, "\""));
		}
		break;

	case T_SRV:
		ND_PRINT((ndo, " "));
		if (!ND_TTEST2(*cp, 6))
			return(NULL);
		if (ns_nprint(ndo, cp + 6, bp) == NULL)
			return(NULL);
		ND_PRINT((ndo, ":%d %d %d", EXTRACT_16BITS(cp + 4),
			EXTRACT_16BITS(cp), EXTRACT_16BITS(cp + 2)));
		break;

	case T_AAAA:
	    {
		char ntop_buf[INET6_ADDRSTRLEN];

		if (!ND_TTEST2(*cp, sizeof(struct in6_addr)))
			return(NULL);
		ND_PRINT((ndo, " %s",
		    addrtostr6(cp, ntop_buf, sizeof(ntop_buf))));

		break;
	    }

	case T_A6:
	    {
		struct in6_addr a;
		int pbit, pbyte;
		char ntop_buf[INET6_ADDRSTRLEN];

		if (!ND_TTEST2(*cp, 1))
			return(NULL);
		pbit = *cp;
		pbyte = (pbit & ~7) / 8;
		if (pbit > 128) {
			ND_PRINT((ndo, " %u(bad plen)", pbit));
			break;
		} else if (pbit < 128) {
			if (!ND_TTEST2(*(cp + 1), sizeof(a) - pbyte))
				return(NULL);
			memset(&a, 0, sizeof(a));
			memcpy(&a.s6_addr[pbyte], cp + 1, sizeof(a) - pbyte);
			ND_PRINT((ndo, " %u %s", pbit,
			    addrtostr6(&a, ntop_buf, sizeof(ntop_buf))));
		}
		if (pbit > 0) {
			ND_PRINT((ndo, " "));
			if (ns_nprint(ndo, cp + 1 + sizeof(a) - pbyte, bp) == NULL)
				return(NULL);
		}
		break;
	    }

	case T_OPT:
		ND_PRINT((ndo, " UDPsize=%u", class));
		if (opt_flags & 0x8000)
			ND_PRINT((ndo, " DO"));
		break;

	case T_UNSPECA:		/* One long string */
		if (!ND_TTEST2(*cp, len))
			return(NULL);
		if (fn_printn(ndo, cp, len, ndo->ndo_snapend))
			return(NULL);
		break;

	case T_TSIG:
	    {
		if (cp + len > ndo->ndo_snapend)
			return(NULL);
		if (!ndo->ndo_vflag)
			break;
		ND_PRINT((ndo, " "));
		if ((cp = ns_nprint(ndo, cp, bp)) == NULL)
			return(NULL);
		cp += 6;
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		ND_PRINT((ndo, " fudge=%u", EXTRACT_16BITS(cp)));
		cp += 2;
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		ND_PRINT((ndo, " maclen=%u", EXTRACT_16BITS(cp)));
		cp += 2 + EXTRACT_16BITS(cp);
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		ND_PRINT((ndo, " origid=%u", EXTRACT_16BITS(cp)));
		cp += 2;
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		ND_PRINT((ndo, " error=%u", EXTRACT_16BITS(cp)));
		cp += 2;
		if (!ND_TTEST2(*cp, 2))
			return(NULL);
		ND_PRINT((ndo, " otherlen=%u", EXTRACT_16BITS(cp)));
		cp += 2;
	    }
	}
	return (rp);		/* XXX This isn't always right */
}
