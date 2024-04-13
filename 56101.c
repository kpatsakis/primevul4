ubik_reply_print(netdissect_options *ndo,
                 register const u_char *bp, int length, int32_t opcode)
{
	const struct rx_header *rxh;

	if (length < (int)sizeof(struct rx_header))
		return;

	rxh = (const struct rx_header *) bp;

	/*
	 * Print out the ubik call we're invoking.  This table was gleaned
	 * from ubik/ubik_int.xg
	 */

	ND_PRINT((ndo, " ubik reply %s", tok2str(ubik_req, "op#%d", opcode)));

	bp += sizeof(struct rx_header);

	/*
	 * If it was a data packet, print out the arguments to the Ubik calls
	 */

	if (rxh->type == RX_PACKET_TYPE_DATA)
		switch (opcode) {
		case 10000:		/* Beacon */
			ND_PRINT((ndo, " vote no"));
			break;
		case 20004:		/* Get version */
			ND_PRINT((ndo, " dbversion"));
			UBIK_VERSIONOUT();
			break;
		default:
			;
		}

	/*
	 * Otherwise, print out "yes" it it was a beacon packet (because
	 * that's how yes votes are returned, go figure), otherwise
	 * just print out the error code.
	 */

	else
		switch (opcode) {
		case 10000:		/* Beacon */
			ND_PRINT((ndo, " vote yes until"));
			DATEOUT();
			break;
		default:
			ND_PRINT((ndo, " errcode"));
			INTOUT();
		}

	return;

trunc:
	ND_PRINT((ndo, " [|ubik]"));
}
