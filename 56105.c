vol_reply_print(netdissect_options *ndo,
                register const u_char *bp, int length, int32_t opcode)
{
	const struct rx_header *rxh;

	if (length <= (int)sizeof(struct rx_header))
		return;

	rxh = (const struct rx_header *) bp;

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from volser/volint.xg
	 */

	ND_PRINT((ndo, " vol reply %s", tok2str(vol_req, "op#%d", opcode)));

	bp += sizeof(struct rx_header);

	/*
	 * If it was a data packet, interpret the response.
	 */

	if (rxh->type == RX_PACKET_TYPE_DATA) {
		switch (opcode) {
			case 100:	/* Create volume */
				ND_PRINT((ndo, " volid"));
				UINTOUT();
				ND_PRINT((ndo, " trans"));
				UINTOUT();
				break;
			case 104:	/* End transaction */
				UINTOUT();
				break;
			case 105:	/* Clone */
				ND_PRINT((ndo, " newvol"));
				UINTOUT();
				break;
			case 107:	/* Get flags */
				UINTOUT();
				break;
			case 108:	/* Transaction create */
				ND_PRINT((ndo, " trans"));
				UINTOUT();
				break;
			case 110:	/* Get n-th volume */
				ND_PRINT((ndo, " volume"));
				UINTOUT();
				ND_PRINT((ndo, " partition"));
				UINTOUT();
				break;
			case 112:	/* Get name */
				STROUT(AFSNAMEMAX);
				break;
			case 113:	/* Get status */
				ND_PRINT((ndo, " volid"));
				UINTOUT();
				ND_PRINT((ndo, " nextuniq"));
				UINTOUT();
				ND_PRINT((ndo, " type"));
				UINTOUT();
				ND_PRINT((ndo, " parentid"));
				UINTOUT();
				ND_PRINT((ndo, " clone"));
				UINTOUT();
				ND_PRINT((ndo, " backup"));
				UINTOUT();
				ND_PRINT((ndo, " restore"));
				UINTOUT();
				ND_PRINT((ndo, " maxquota"));
				UINTOUT();
				ND_PRINT((ndo, " minquota"));
				UINTOUT();
				ND_PRINT((ndo, " owner"));
				UINTOUT();
				ND_PRINT((ndo, " create"));
				DATEOUT();
				ND_PRINT((ndo, " access"));
				DATEOUT();
				ND_PRINT((ndo, " update"));
				DATEOUT();
				ND_PRINT((ndo, " expire"));
				DATEOUT();
				ND_PRINT((ndo, " backup"));
				DATEOUT();
				ND_PRINT((ndo, " copy"));
				DATEOUT();
				break;
			case 115:	/* Old list partitions */
				break;
			case 116:	/* List volumes */
			case 121:	/* List one volume */
				{
					unsigned long i, j;
					ND_TCHECK2(bp[0], 4);
					j = EXTRACT_32BITS(bp);
					bp += sizeof(int32_t);
					for (i = 0; i < j; i++) {
						ND_PRINT((ndo, " name"));
						VECOUT(32);
						ND_PRINT((ndo, " volid"));
						UINTOUT();
						ND_PRINT((ndo, " type"));
						bp += sizeof(int32_t) * 21;
						if (i != j - 1)
							ND_PRINT((ndo, ","));
					}
					if (j == 0)
						ND_PRINT((ndo, " <none!>"));
				}
				break;


			default:
				;
		}
	} else {
		/*
		 * Otherwise, just print out the return code
		 */
		ND_PRINT((ndo, " errcode"));
		INTOUT();
	}

	return;

trunc:
	ND_PRINT((ndo, " [|vol]"));
}
