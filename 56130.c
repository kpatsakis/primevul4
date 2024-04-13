ip_print_demux(netdissect_options *ndo,
	       struct ip_print_demux_state *ipds)
{
	const char *p_name;

again:
	switch (ipds->nh) {

	case IPPROTO_AH:
		if (!ND_TTEST(*ipds->cp)) {
			ND_PRINT((ndo, "[|AH]"));
			break;
		}
		ipds->nh = *ipds->cp;
		ipds->advance = ah_print(ndo, ipds->cp);
		if (ipds->advance <= 0)
			break;
		ipds->cp += ipds->advance;
		ipds->len -= ipds->advance;
		goto again;

	case IPPROTO_ESP:
	{
		int enh, padlen;
		ipds->advance = esp_print(ndo, ipds->cp, ipds->len,
				    (const u_char *)ipds->ip,
				    &enh, &padlen);
		if (ipds->advance <= 0)
			break;
		ipds->cp += ipds->advance;
		ipds->len -= ipds->advance + padlen;
		ipds->nh = enh & 0xff;
		goto again;
	}

	case IPPROTO_IPCOMP:
	{
		ipcomp_print(ndo, ipds->cp);
		/*
		 * Either this has decompressed the payload and
		 * printed it, in which case there's nothing more
		 * to do, or it hasn't, in which case there's
		 * nothing more to do.
		 */
		break;
	}

	case IPPROTO_SCTP:
		sctp_print(ndo, ipds->cp, (const u_char *)ipds->ip, ipds->len);
		break;

	case IPPROTO_DCCP:
		dccp_print(ndo, ipds->cp, (const u_char *)ipds->ip, ipds->len);
		break;

	case IPPROTO_TCP:
		/* pass on the MF bit plus the offset to detect fragments */
		tcp_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip,
			  ipds->off & (IP_MF|IP_OFFMASK));
		break;

	case IPPROTO_UDP:
		/* pass on the MF bit plus the offset to detect fragments */
		udp_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip,
			  ipds->off & (IP_MF|IP_OFFMASK));
		break;

	case IPPROTO_ICMP:
		/* pass on the MF bit plus the offset to detect fragments */
		icmp_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip,
			   ipds->off & (IP_MF|IP_OFFMASK));
		break;

	case IPPROTO_PIGP:
		/*
		 * XXX - the current IANA protocol number assignments
		 * page lists 9 as "any private interior gateway
		 * (used by Cisco for their IGRP)" and 88 as
		 * "EIGRP" from Cisco.
		 *
		 * Recent BSD <netinet/in.h> headers define
		 * IP_PROTO_PIGP as 9 and IP_PROTO_IGRP as 88.
		 * We define IP_PROTO_PIGP as 9 and
		 * IP_PROTO_EIGRP as 88; those names better
		 * match was the current protocol number
		 * assignments say.
		 */
		igrp_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_EIGRP:
		eigrp_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_ND:
		ND_PRINT((ndo, " nd %d", ipds->len));
		break;

	case IPPROTO_EGP:
		egp_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_OSPF:
		ospf_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip);
		break;

	case IPPROTO_IGMP:
		igmp_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_IPV4:
		/* DVMRP multicast tunnel (ip-in-ip encapsulation) */
		ip_print(ndo, ipds->cp, ipds->len);
		if (! ndo->ndo_vflag) {
			ND_PRINT((ndo, " (ipip-proto-4)"));
			return;
		}
		break;

	case IPPROTO_IPV6:
		/* ip6-in-ip encapsulation */
		ip6_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_RSVP:
		rsvp_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_GRE:
		/* do it */
		gre_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_MOBILE:
		mobile_print(ndo, ipds->cp, ipds->len);
		break;

	case IPPROTO_PIM:
		pim_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip);
		break;

	case IPPROTO_VRRP:
		if (ndo->ndo_packettype == PT_CARP) {
			if (ndo->ndo_vflag)
				ND_PRINT((ndo, "carp %s > %s: ",
					     ipaddr_string(ndo, &ipds->ip->ip_src),
					     ipaddr_string(ndo, &ipds->ip->ip_dst)));
			carp_print(ndo, ipds->cp, ipds->len, ipds->ip->ip_ttl);
		} else {
			if (ndo->ndo_vflag)
				ND_PRINT((ndo, "vrrp %s > %s: ",
					     ipaddr_string(ndo, &ipds->ip->ip_src),
					     ipaddr_string(ndo, &ipds->ip->ip_dst)));
			vrrp_print(ndo, ipds->cp, ipds->len,
				(const u_char *)ipds->ip, ipds->ip->ip_ttl);
		}
		break;

	case IPPROTO_PGM:
		pgm_print(ndo, ipds->cp, ipds->len, (const u_char *)ipds->ip);
		break;

	default:
		if (ndo->ndo_nflag==0 && (p_name = netdb_protoname(ipds->nh)) != NULL)
			ND_PRINT((ndo, " %s", p_name));
		else
			ND_PRINT((ndo, " ip-proto-%d", ipds->nh));
		ND_PRINT((ndo, " %d", ipds->len));
		break;
	}
}
