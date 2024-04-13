ospf6_print_lsa(netdissect_options *ndo,
                register const struct lsa6 *lsap, const u_char *dataend)
{
	register const struct rlalink6 *rlp;
#if 0
	register const struct tos_metric *tosp;
#endif
	register const rtrid_t *ap;
#if 0
	register const struct aslametric *almp;
	register const struct mcla *mcp;
#endif
	register const struct llsa *llsap;
	register const struct lsa6_prefix *lsapp;
#if 0
	register const uint32_t *lp;
#endif
	register u_int prefixes;
	register int bytelen;
	register u_int length, lsa_length;
	uint32_t flags32;
	const uint8_t *tptr;

	if (ospf6_print_lshdr(ndo, &lsap->ls_hdr, dataend))
		return (1);
	ND_TCHECK(lsap->ls_hdr.ls_length);
        length = EXTRACT_16BITS(&lsap->ls_hdr.ls_length);

	/*
	 * The LSA length includes the length of the header;
	 * it must have a value that's at least that length.
	 * If it does, find the length of what follows the
	 * header.
	 */
        if (length < sizeof(struct lsa6_hdr) || (const u_char *)lsap + length > dataend)
        	return (1);
        lsa_length = length - sizeof(struct lsa6_hdr);
        tptr = (const uint8_t *)lsap+sizeof(struct lsa6_hdr);

	switch (EXTRACT_16BITS(&lsap->ls_hdr.ls_type)) {
	case LS_TYPE_ROUTER | LS_SCOPE_AREA:
		if (lsa_length < sizeof (lsap->lsa_un.un_rla.rla_options))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_rla.rla_options);
		ND_TCHECK(lsap->lsa_un.un_rla.rla_options);
		ND_PRINT((ndo, "\n\t      Options [%s]",
		          bittok2str(ospf6_option_values, "none",
		          EXTRACT_32BITS(&lsap->lsa_un.un_rla.rla_options))));
		ND_PRINT((ndo, ", RLA-Flags [%s]",
		          bittok2str(ospf6_rla_flag_values, "none",
		          lsap->lsa_un.un_rla.rla_flags)));

		rlp = lsap->lsa_un.un_rla.rla_link;
		while (lsa_length != 0) {
			if (lsa_length < sizeof (*rlp))
				return (1);
			lsa_length -= sizeof (*rlp);
			ND_TCHECK(*rlp);
			switch (rlp->link_type) {

			case RLA_TYPE_VIRTUAL:
				ND_PRINT((ndo, "\n\t      Virtual Link: Neighbor Router-ID %s"
                                       "\n\t      Neighbor Interface-ID %s, Interface %s",
                                       ipaddr_string(ndo, &rlp->link_nrtid),
                                       ipaddr_string(ndo, &rlp->link_nifid),
                                       ipaddr_string(ndo, &rlp->link_ifid)));
                                break;

			case RLA_TYPE_ROUTER:
				ND_PRINT((ndo, "\n\t      Neighbor Router-ID %s"
                                       "\n\t      Neighbor Interface-ID %s, Interface %s",
                                       ipaddr_string(ndo, &rlp->link_nrtid),
                                       ipaddr_string(ndo, &rlp->link_nifid),
                                       ipaddr_string(ndo, &rlp->link_ifid)));
				break;

			case RLA_TYPE_TRANSIT:
				ND_PRINT((ndo, "\n\t      Neighbor Network-ID %s"
                                       "\n\t      Neighbor Interface-ID %s, Interface %s",
				    ipaddr_string(ndo, &rlp->link_nrtid),
				    ipaddr_string(ndo, &rlp->link_nifid),
				    ipaddr_string(ndo, &rlp->link_ifid)));
				break;

			default:
				ND_PRINT((ndo, "\n\t      Unknown Router Links Type 0x%02x",
				    rlp->link_type));
				return (0);
			}
			ND_PRINT((ndo, ", metric %d", EXTRACT_16BITS(&rlp->link_metric)));
			rlp++;
		}
		break;

	case LS_TYPE_NETWORK | LS_SCOPE_AREA:
		if (lsa_length < sizeof (lsap->lsa_un.un_nla.nla_options))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_nla.nla_options);
		ND_TCHECK(lsap->lsa_un.un_nla.nla_options);
		ND_PRINT((ndo, "\n\t      Options [%s]",
		          bittok2str(ospf6_option_values, "none",
		          EXTRACT_32BITS(&lsap->lsa_un.un_nla.nla_options))));

		ND_PRINT((ndo, "\n\t      Connected Routers:"));
		ap = lsap->lsa_un.un_nla.nla_router;
		while (lsa_length != 0) {
			if (lsa_length < sizeof (*ap))
				return (1);
			lsa_length -= sizeof (*ap);
			ND_TCHECK(*ap);
			ND_PRINT((ndo, "\n\t\t%s", ipaddr_string(ndo, ap)));
			++ap;
		}
		break;

	case LS_TYPE_INTER_AP | LS_SCOPE_AREA:
		if (lsa_length < sizeof (lsap->lsa_un.un_inter_ap.inter_ap_metric))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_inter_ap.inter_ap_metric);
		ND_TCHECK(lsap->lsa_un.un_inter_ap.inter_ap_metric);
		ND_PRINT((ndo, ", metric %u",
			EXTRACT_32BITS(&lsap->lsa_un.un_inter_ap.inter_ap_metric) & SLA_MASK_METRIC));

		tptr = (const uint8_t *)lsap->lsa_un.un_inter_ap.inter_ap_prefix;
		while (lsa_length != 0) {
			bytelen = ospf6_print_lsaprefix(ndo, tptr, lsa_length);
			if (bytelen < 0)
				goto trunc;
			lsa_length -= bytelen;
			tptr += bytelen;
		}
		break;

	case LS_TYPE_ASE | LS_SCOPE_AS:
		if (lsa_length < sizeof (lsap->lsa_un.un_asla.asla_metric))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_asla.asla_metric);
		ND_TCHECK(lsap->lsa_un.un_asla.asla_metric);
		flags32 = EXTRACT_32BITS(&lsap->lsa_un.un_asla.asla_metric);
		ND_PRINT((ndo, "\n\t     Flags [%s]",
		          bittok2str(ospf6_asla_flag_values, "none", flags32)));
		ND_PRINT((ndo, " metric %u",
		       EXTRACT_32BITS(&lsap->lsa_un.un_asla.asla_metric) &
		       ASLA_MASK_METRIC));

		tptr = (const uint8_t *)lsap->lsa_un.un_asla.asla_prefix;
		lsapp = (const struct lsa6_prefix *)tptr;
		bytelen = ospf6_print_lsaprefix(ndo, tptr, lsa_length);
		if (bytelen < 0)
			goto trunc;
		lsa_length -= bytelen;
		tptr += bytelen;

		if ((flags32 & ASLA_FLAG_FWDADDR) != 0) {
			const struct in6_addr *fwdaddr6;

			fwdaddr6 = (const struct in6_addr *)tptr;
			if (lsa_length < sizeof (*fwdaddr6))
				return (1);
			lsa_length -= sizeof (*fwdaddr6);
			ND_TCHECK(*fwdaddr6);
			ND_PRINT((ndo, " forward %s",
			       ip6addr_string(ndo, fwdaddr6)));
			tptr += sizeof(*fwdaddr6);
		}

		if ((flags32 & ASLA_FLAG_ROUTETAG) != 0) {
			if (lsa_length < sizeof (uint32_t))
				return (1);
			lsa_length -= sizeof (uint32_t);
			ND_TCHECK(*(const uint32_t *)tptr);
			ND_PRINT((ndo, " tag %s",
			       ipaddr_string(ndo, (const uint32_t *)tptr)));
			tptr += sizeof(uint32_t);
		}

		if (lsapp->lsa_p_metric) {
			if (lsa_length < sizeof (uint32_t))
				return (1);
			lsa_length -= sizeof (uint32_t);
			ND_TCHECK(*(const uint32_t *)tptr);
			ND_PRINT((ndo, " RefLSID: %s",
			       ipaddr_string(ndo, (const uint32_t *)tptr)));
			tptr += sizeof(uint32_t);
		}
		break;

	case LS_TYPE_LINK:
		/* Link LSA */
		llsap = &lsap->lsa_un.un_llsa;
		if (lsa_length < sizeof (llsap->llsa_priandopt))
			return (1);
		lsa_length -= sizeof (llsap->llsa_priandopt);
		ND_TCHECK(llsap->llsa_priandopt);
		ND_PRINT((ndo, "\n\t      Options [%s]",
		          bittok2str(ospf6_option_values, "none",
		          EXTRACT_32BITS(&llsap->llsa_options))));

		if (lsa_length < sizeof (llsap->llsa_lladdr) + sizeof (llsap->llsa_nprefix))
			return (1);
		lsa_length -= sizeof (llsap->llsa_lladdr) + sizeof (llsap->llsa_nprefix);
                prefixes = EXTRACT_32BITS(&llsap->llsa_nprefix);
		ND_PRINT((ndo, "\n\t      Priority %d, Link-local address %s, Prefixes %d:",
                       llsap->llsa_priority,
                       ip6addr_string(ndo, &llsap->llsa_lladdr),
                       prefixes));

		tptr = (const uint8_t *)llsap->llsa_prefix;
		while (prefixes > 0) {
			bytelen = ospf6_print_lsaprefix(ndo, tptr, lsa_length);
			if (bytelen < 0)
				goto trunc;
			prefixes--;
			lsa_length -= bytelen;
			tptr += bytelen;
		}
		break;

	case LS_TYPE_INTRA_AP | LS_SCOPE_AREA:
		/* Intra-Area-Prefix LSA */
		if (lsa_length < sizeof (lsap->lsa_un.un_intra_ap.intra_ap_rtid))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_intra_ap.intra_ap_rtid);
		ND_TCHECK(lsap->lsa_un.un_intra_ap.intra_ap_rtid);
		ospf6_print_ls_type(ndo,
			EXTRACT_16BITS(&lsap->lsa_un.un_intra_ap.intra_ap_lstype),
			&lsap->lsa_un.un_intra_ap.intra_ap_lsid);

		if (lsa_length < sizeof (lsap->lsa_un.un_intra_ap.intra_ap_nprefix))
			return (1);
		lsa_length -= sizeof (lsap->lsa_un.un_intra_ap.intra_ap_nprefix);
		ND_TCHECK(lsap->lsa_un.un_intra_ap.intra_ap_nprefix);
                prefixes = EXTRACT_16BITS(&lsap->lsa_un.un_intra_ap.intra_ap_nprefix);
		ND_PRINT((ndo, "\n\t      Prefixes %d:", prefixes));

		tptr = (const uint8_t *)lsap->lsa_un.un_intra_ap.intra_ap_prefix;
		while (prefixes > 0) {
			bytelen = ospf6_print_lsaprefix(ndo, tptr, lsa_length);
			if (bytelen < 0)
				goto trunc;
			prefixes--;
			lsa_length -= bytelen;
			tptr += bytelen;
		}
		break;

        case LS_TYPE_GRACE | LS_SCOPE_LINKLOCAL:
                if (ospf_print_grace_lsa(ndo, tptr, lsa_length) == -1) {
                    return 1;
                }
                break;

        case LS_TYPE_INTRA_ATE | LS_SCOPE_LINKLOCAL:
                if (ospf_print_te_lsa(ndo, tptr, lsa_length) == -1) {
                    return 1;
                }
                break;

	default:
                if(!print_unknown_data(ndo,tptr,
                                       "\n\t      ",
                                       lsa_length)) {
                    return (1);
                }
                break;
	}

	return (0);
trunc:
	return (1);
}
