bgp_header_print(netdissect_options *ndo,
                 const u_char *dat, int length)
{
	struct bgp bgp;

	ND_TCHECK2(dat[0], BGP_SIZE);
	memcpy(&bgp, dat, BGP_SIZE);
	ND_PRINT((ndo, "\n\t%s Message (%u), length: %u",
               tok2str(bgp_msg_values, "Unknown", bgp.bgp_type),
               bgp.bgp_type,
               length));

	switch (bgp.bgp_type) {
	case BGP_OPEN:
		bgp_open_print(ndo, dat, length);
		break;
	case BGP_UPDATE:
		bgp_update_print(ndo, dat, length);
		break;
	case BGP_NOTIFICATION:
		bgp_notification_print(ndo, dat, length);
		break;
        case BGP_KEEPALIVE:
                break;
        case BGP_ROUTE_REFRESH:
                bgp_route_refresh_print(ndo, dat, length);
                break;
        default:
                /* we have no decoder for the BGP message */
                ND_TCHECK2(*dat, length);
                ND_PRINT((ndo, "\n\t  no Message %u decoder", bgp.bgp_type));
                print_unknown_data(ndo, dat, "\n\t  ", length);
                break;
	}
	return 1;
trunc:
	ND_PRINT((ndo, "[|BGP]"));
	return 0;
}
