vldb_print(netdissect_options *ndo,
           register const u_char *bp, int length)
{
	int vldb_op;
	unsigned long i;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from vlserver/vldbint.xg
	 */

	vldb_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " vldb"));

	if (is_ubik(vldb_op)) {
		ubik_print(ndo, bp);
		return;
	}
	ND_PRINT((ndo, " call %s", tok2str(vldb_req, "op#%d", vldb_op)));

	/*
	 * Decode some of the arguments to the VLDB calls
	 */

	bp += sizeof(struct rx_header) + 4;

	switch (vldb_op) {
		case 501:	/* Create new volume */
		case 517:	/* Create entry N */
			VECOUT(VLNAMEMAX);
			break;
		case 502:	/* Delete entry */
		case 503:	/* Get entry by ID */
		case 507:	/* Update entry */
		case 508:	/* Set lock */
		case 509:	/* Release lock */
		case 518:	/* Get entry by ID N */
			ND_PRINT((ndo, " volid"));
			INTOUT();
			ND_TCHECK2(bp[0], sizeof(int32_t));
			i = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);
			if (i <= 2)
				ND_PRINT((ndo, " type %s", voltype[i]));
			break;
		case 504:	/* Get entry by name */
		case 519:	/* Get entry by name N */
		case 524:	/* Update entry by name */
		case 527:	/* Get entry by name U */
			STROUT(VLNAMEMAX);
			break;
		case 505:	/* Get new vol id */
			ND_PRINT((ndo, " bump"));
			INTOUT();
			break;
		case 506:	/* Replace entry */
		case 520:	/* Replace entry N */
			ND_PRINT((ndo, " volid"));
			INTOUT();
			ND_TCHECK2(bp[0], sizeof(int32_t));
			i = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);
			if (i <= 2)
				ND_PRINT((ndo, " type %s", voltype[i]));
			VECOUT(VLNAMEMAX);
			break;
		case 510:	/* List entry */
		case 521:	/* List entry N */
			ND_PRINT((ndo, " index"));
			INTOUT();
			break;
		default:
			;
	}

	return;

trunc:
	ND_PRINT((ndo, " [|vldb]"));
}
