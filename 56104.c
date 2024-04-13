vol_print(netdissect_options *ndo,
          register const u_char *bp, int length)
{
	int vol_op;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from volser/volint.xg
	 */

	vol_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " vol call %s", tok2str(vol_req, "op#%d", vol_op)));

	bp += sizeof(struct rx_header) + 4;

	switch (vol_op) {
		case 100:	/* Create volume */
			ND_PRINT((ndo, " partition"));
			UINTOUT();
			ND_PRINT((ndo, " name"));
			STROUT(AFSNAMEMAX);
			ND_PRINT((ndo, " type"));
			UINTOUT();
			ND_PRINT((ndo, " parent"));
			UINTOUT();
			break;
		case 101:	/* Delete volume */
		case 107:	/* Get flags */
			ND_PRINT((ndo, " trans"));
			UINTOUT();
			break;
		case 102:	/* Restore */
			ND_PRINT((ndo, " totrans"));
			UINTOUT();
			ND_PRINT((ndo, " flags"));
			UINTOUT();
			break;
		case 103:	/* Forward */
			ND_PRINT((ndo, " fromtrans"));
			UINTOUT();
			ND_PRINT((ndo, " fromdate"));
			DATEOUT();
			DESTSERVEROUT();
			ND_PRINT((ndo, " desttrans"));
			INTOUT();
			break;
		case 104:	/* End trans */
			ND_PRINT((ndo, " trans"));
			UINTOUT();
			break;
		case 105:	/* Clone */
			ND_PRINT((ndo, " trans"));
			UINTOUT();
			ND_PRINT((ndo, " purgevol"));
			UINTOUT();
			ND_PRINT((ndo, " newtype"));
			UINTOUT();
			ND_PRINT((ndo, " newname"));
			STROUT(AFSNAMEMAX);
			break;
		case 106:	/* Set flags */
			ND_PRINT((ndo, " trans"));
			UINTOUT();
			ND_PRINT((ndo, " flags"));
			UINTOUT();
			break;
		case 108:	/* Trans create */
			ND_PRINT((ndo, " vol"));
			UINTOUT();
			ND_PRINT((ndo, " partition"));
			UINTOUT();
			ND_PRINT((ndo, " flags"));
			UINTOUT();
			break;
		case 109:	/* Dump */
		case 655537:	/* Get size */
			ND_PRINT((ndo, " fromtrans"));
			UINTOUT();
			ND_PRINT((ndo, " fromdate"));
			DATEOUT();
			break;
		case 110:	/* Get n-th volume */
			ND_PRINT((ndo, " index"));
			UINTOUT();
			break;
		case 111:	/* Set forwarding */
			ND_PRINT((ndo, " tid"));
			UINTOUT();
			ND_PRINT((ndo, " newsite"));
			UINTOUT();
			break;
		case 112:	/* Get name */
		case 113:	/* Get status */
			ND_PRINT((ndo, " tid"));
			break;
		case 114:	/* Signal restore */
			ND_PRINT((ndo, " name"));
			STROUT(AFSNAMEMAX);
			ND_PRINT((ndo, " type"));
			UINTOUT();
			ND_PRINT((ndo, " pid"));
			UINTOUT();
			ND_PRINT((ndo, " cloneid"));
			UINTOUT();
			break;
		case 116:	/* List volumes */
			ND_PRINT((ndo, " partition"));
			UINTOUT();
			ND_PRINT((ndo, " flags"));
			UINTOUT();
			break;
		case 117:	/* Set id types */
			ND_PRINT((ndo, " tid"));
			UINTOUT();
			ND_PRINT((ndo, " name"));
			STROUT(AFSNAMEMAX);
			ND_PRINT((ndo, " type"));
			UINTOUT();
			ND_PRINT((ndo, " pid"));
			UINTOUT();
			ND_PRINT((ndo, " clone"));
			UINTOUT();
			ND_PRINT((ndo, " backup"));
			UINTOUT();
			break;
		case 119:	/* Partition info */
			ND_PRINT((ndo, " name"));
			STROUT(AFSNAMEMAX);
			break;
		case 120:	/* Reclone */
			ND_PRINT((ndo, " tid"));
			UINTOUT();
			break;
		case 121:	/* List one volume */
		case 122:	/* Nuke volume */
		case 124:	/* Extended List volumes */
		case 125:	/* Extended List one volume */
		case 65536:	/* Convert RO to RW volume */
			ND_PRINT((ndo, " partid"));
			UINTOUT();
			ND_PRINT((ndo, " volid"));
			UINTOUT();
			break;
		case 123:	/* Set date */
			ND_PRINT((ndo, " tid"));
			UINTOUT();
			ND_PRINT((ndo, " date"));
			DATEOUT();
			break;
		case 126:	/* Set info */
			ND_PRINT((ndo, " tid"));
			UINTOUT();
			break;
		case 128:	/* Forward multiple */
			ND_PRINT((ndo, " fromtrans"));
			UINTOUT();
			ND_PRINT((ndo, " fromdate"));
			DATEOUT();
			{
				unsigned long i, j;
				ND_TCHECK2(bp[0], 4);
				j = EXTRACT_32BITS(bp);
				bp += sizeof(int32_t);
				for (i = 0; i < j; i++) {
					DESTSERVEROUT();
					if (i != j - 1)
						ND_PRINT((ndo, ","));
				}
				if (j == 0)
					ND_PRINT((ndo, " <none!>"));
			}
			break;
		case 65538:	/* Dump version 2 */
			ND_PRINT((ndo, " fromtrans"));
			UINTOUT();
			ND_PRINT((ndo, " fromdate"));
			DATEOUT();
			ND_PRINT((ndo, " flags"));
			UINTOUT();
			break;
		default:
			;
	}
	return;

trunc:
	ND_PRINT((ndo, " [|vol]"));
}
