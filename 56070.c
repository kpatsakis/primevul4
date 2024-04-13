bgp_notification_print(netdissect_options *ndo,
                       const u_char *dat, int length)
{
	struct bgp_notification bgpn;
	const u_char *tptr;
	uint8_t shutdown_comm_length;
	uint8_t remainder_offset;

	ND_TCHECK2(dat[0], BGP_NOTIFICATION_SIZE);
	memcpy(&bgpn, dat, BGP_NOTIFICATION_SIZE);

        /* some little sanity checking */
        if (length<BGP_NOTIFICATION_SIZE)
            return;

	ND_PRINT((ndo, ", %s (%u)",
	       tok2str(bgp_notify_major_values, "Unknown Error",
			  bgpn.bgpn_major),
	       bgpn.bgpn_major));

        switch (bgpn.bgpn_major) {

        case BGP_NOTIFY_MAJOR_MSG:
            ND_PRINT((ndo, ", subcode %s (%u)",
		   tok2str(bgp_notify_minor_msg_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));
            break;
        case BGP_NOTIFY_MAJOR_OPEN:
            ND_PRINT((ndo, ", subcode %s (%u)",
		   tok2str(bgp_notify_minor_open_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));
            break;
        case BGP_NOTIFY_MAJOR_UPDATE:
            ND_PRINT((ndo, ", subcode %s (%u)",
		   tok2str(bgp_notify_minor_update_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));
            break;
        case BGP_NOTIFY_MAJOR_FSM:
            ND_PRINT((ndo, " subcode %s (%u)",
		   tok2str(bgp_notify_minor_fsm_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));
            break;
        case BGP_NOTIFY_MAJOR_CAP:
            ND_PRINT((ndo, " subcode %s (%u)",
		   tok2str(bgp_notify_minor_cap_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));
            break;
        case BGP_NOTIFY_MAJOR_CEASE:
            ND_PRINT((ndo, ", subcode %s (%u)",
		   tok2str(bgp_notify_minor_cease_values, "Unknown",
			      bgpn.bgpn_minor),
		   bgpn.bgpn_minor));

	    /* draft-ietf-idr-cease-subcode-02 mentions optionally 7 bytes
             * for the maxprefix subtype, which may contain AFI, SAFI and MAXPREFIXES
             */
	    if(bgpn.bgpn_minor == BGP_NOTIFY_MINOR_CEASE_MAXPRFX && length >= BGP_NOTIFICATION_SIZE + 7) {
		tptr = dat + BGP_NOTIFICATION_SIZE;
		ND_TCHECK2(*tptr, 7);
		ND_PRINT((ndo, ", AFI %s (%u), SAFI %s (%u), Max Prefixes: %u",
		       tok2str(af_values, "Unknown",
				  EXTRACT_16BITS(tptr)),
		       EXTRACT_16BITS(tptr),
		       tok2str(bgp_safi_values, "Unknown", *(tptr+2)),
		       *(tptr+2),
		       EXTRACT_32BITS(tptr+3)));
	    }
	    /*
	     * draft-ietf-idr-shutdown describes a method to send a communication
	     * intended for human consumption regarding the Administrative Shutdown
	     */
	    if ((bgpn.bgpn_minor == BGP_NOTIFY_MINOR_CEASE_SHUT ||
		bgpn.bgpn_minor == BGP_NOTIFY_MINOR_CEASE_RESET) &&
		length >= BGP_NOTIFICATION_SIZE + 1) {
		    tptr = dat + BGP_NOTIFICATION_SIZE;
		    ND_TCHECK2(*tptr, 1);
		    shutdown_comm_length = *(tptr);
		    remainder_offset = 0;
		    /* garbage, hexdump it all */
		    if (shutdown_comm_length > BGP_NOTIFY_MINOR_CEASE_ADMIN_SHUTDOWN_LEN ||
			shutdown_comm_length > length - (BGP_NOTIFICATION_SIZE + 1)) {
			    ND_PRINT((ndo, ", invalid Shutdown Communication length"));
		    }
		    else if (shutdown_comm_length == 0) {
			    ND_PRINT((ndo, ", empty Shutdown Communication"));
			    remainder_offset += 1;
		    }
		    /* a proper shutdown communication */
		    else {
			    ND_TCHECK2(*(tptr+1), shutdown_comm_length);
			    ND_PRINT((ndo, ", Shutdown Communication (length: %u): \"", shutdown_comm_length));
			    (void)fn_printn(ndo, tptr+1, shutdown_comm_length, NULL);
			    ND_PRINT((ndo, "\""));
			    remainder_offset += shutdown_comm_length + 1;
		    }
		    /* if there is trailing data, hexdump it */
		    if(length - (remainder_offset + BGP_NOTIFICATION_SIZE) > 0) {
			    ND_PRINT((ndo, ", Data: (length: %u)", length - (remainder_offset + BGP_NOTIFICATION_SIZE)));
			    hex_print(ndo, "\n\t\t", tptr + remainder_offset, length - (remainder_offset + BGP_NOTIFICATION_SIZE));
		    }
	    }
	    break;
	default:
            break;
        }

	return;
trunc:
	ND_PRINT((ndo, "[|BGP]"));
}
