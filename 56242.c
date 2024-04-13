ns_print(netdissect_options *ndo,
         register const u_char *bp, u_int length, int is_mdns)
{
	register const HEADER *np;
	register int qdcount, ancount, nscount, arcount;
	register const u_char *cp;
	uint16_t b2;

	np = (const HEADER *)bp;
	ND_TCHECK(*np);
	/* get the byte-order right */
	qdcount = EXTRACT_16BITS(&np->qdcount);
	ancount = EXTRACT_16BITS(&np->ancount);
	nscount = EXTRACT_16BITS(&np->nscount);
	arcount = EXTRACT_16BITS(&np->arcount);

	if (DNS_QR(np)) {
		/* this is a response */
		ND_PRINT((ndo, "%d%s%s%s%s%s%s",
			EXTRACT_16BITS(&np->id),
			ns_ops[DNS_OPCODE(np)],
			ns_resp[DNS_RCODE(np)],
			DNS_AA(np)? "*" : "",
			DNS_RA(np)? "" : "-",
			DNS_TC(np)? "|" : "",
			DNS_AD(np)? "$" : ""));

		if (qdcount != 1)
			ND_PRINT((ndo, " [%dq]", qdcount));
		/* Print QUESTION section on -vv */
		cp = (const u_char *)(np + 1);
		while (qdcount--) {
			if (qdcount < EXTRACT_16BITS(&np->qdcount) - 1)
				ND_PRINT((ndo, ","));
			if (ndo->ndo_vflag > 1) {
				ND_PRINT((ndo, " q:"));
				if ((cp = ns_qprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
			} else {
				if ((cp = ns_nskip(ndo, cp)) == NULL)
					goto trunc;
				cp += 4;	/* skip QTYPE and QCLASS */
			}
		}
		ND_PRINT((ndo, " %d/%d/%d", ancount, nscount, arcount));
		if (ancount--) {
			if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
				goto trunc;
			while (cp < ndo->ndo_snapend && ancount--) {
				ND_PRINT((ndo, ","));
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
			}
		}
		if (ancount > 0)
			goto trunc;
		/* Print NS and AR sections on -vv */
		if (ndo->ndo_vflag > 1) {
			if (cp < ndo->ndo_snapend && nscount--) {
				ND_PRINT((ndo, " ns:"));
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
				while (cp < ndo->ndo_snapend && nscount--) {
					ND_PRINT((ndo, ","));
					if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
						goto trunc;
				}
			}
			if (nscount > 0)
				goto trunc;
			if (cp < ndo->ndo_snapend && arcount--) {
				ND_PRINT((ndo, " ar:"));
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
				while (cp < ndo->ndo_snapend && arcount--) {
					ND_PRINT((ndo, ","));
					if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
						goto trunc;
				}
			}
			if (arcount > 0)
				goto trunc;
		}
	}
	else {
		/* this is a request */
		ND_PRINT((ndo, "%d%s%s%s", EXTRACT_16BITS(&np->id), ns_ops[DNS_OPCODE(np)],
		    DNS_RD(np) ? "+" : "",
		    DNS_CD(np) ? "%" : ""));

		/* any weirdness? */
		b2 = EXTRACT_16BITS(((const u_short *)np)+1);
		if (b2 & 0x6cf)
			ND_PRINT((ndo, " [b2&3=0x%x]", b2));

		if (DNS_OPCODE(np) == IQUERY) {
			if (qdcount)
				ND_PRINT((ndo, " [%dq]", qdcount));
			if (ancount != 1)
				ND_PRINT((ndo, " [%da]", ancount));
		}
		else {
			if (ancount)
				ND_PRINT((ndo, " [%da]", ancount));
			if (qdcount != 1)
				ND_PRINT((ndo, " [%dq]", qdcount));
		}
		if (nscount)
			ND_PRINT((ndo, " [%dn]", nscount));
		if (arcount)
			ND_PRINT((ndo, " [%dau]", arcount));

		cp = (const u_char *)(np + 1);
		if (qdcount--) {
			cp = ns_qprint(ndo, cp, (const u_char *)np, is_mdns);
			if (!cp)
				goto trunc;
			while (cp < ndo->ndo_snapend && qdcount--) {
				cp = ns_qprint(ndo, (const u_char *)cp,
					       (const u_char *)np,
					       is_mdns);
				if (!cp)
					goto trunc;
			}
		}
		if (qdcount > 0)
			goto trunc;

		/* Print remaining sections on -vv */
		if (ndo->ndo_vflag > 1) {
			if (ancount--) {
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
				while (cp < ndo->ndo_snapend && ancount--) {
					ND_PRINT((ndo, ","));
					if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
						goto trunc;
				}
			}
			if (ancount > 0)
				goto trunc;
			if (cp < ndo->ndo_snapend && nscount--) {
				ND_PRINT((ndo, " ns:"));
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
				while (nscount-- && cp < ndo->ndo_snapend) {
					ND_PRINT((ndo, ","));
					if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
						goto trunc;
				}
			}
			if (nscount > 0)
				goto trunc;
			if (cp < ndo->ndo_snapend && arcount--) {
				ND_PRINT((ndo, " ar:"));
				if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
					goto trunc;
				while (cp < ndo->ndo_snapend && arcount--) {
					ND_PRINT((ndo, ","));
					if ((cp = ns_rprint(ndo, cp, bp, is_mdns)) == NULL)
						goto trunc;
				}
			}
			if (arcount > 0)
				goto trunc;
		}
	}
	ND_PRINT((ndo, " (%d)", length));
	return;

  trunc:
	ND_PRINT((ndo, "[|domain]"));
}
