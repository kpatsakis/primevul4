kauth_print(netdissect_options *ndo,
            register const u_char *bp, int length)
{
	int kauth_op;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from kauth/kauth.rg
	 */

	kauth_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " kauth"));

	if (is_ubik(kauth_op)) {
		ubik_print(ndo, bp);
		return;
	}


	ND_PRINT((ndo, " call %s", tok2str(kauth_req, "op#%d", kauth_op)));

	/*
	 * Decode some of the arguments to the KA calls
	 */

	bp += sizeof(struct rx_header) + 4;

	switch (kauth_op) {
		case 1:		/* Authenticate old */
		case 21:	/* Authenticate */
		case 22:	/* Authenticate-V2 */
		case 2:		/* Change PW */
		case 5:		/* Set fields */
		case 6:		/* Create user */
		case 7:		/* Delete user */
		case 8:		/* Get entry */
		case 14:	/* Unlock */
		case 15:	/* Lock status */
			ND_PRINT((ndo, " principal"));
			STROUT(KANAMEMAX);
			STROUT(KANAMEMAX);
			break;
		case 3:		/* GetTicket-old */
		case 23:	/* GetTicket */
		{
			int i;
			ND_PRINT((ndo, " kvno"));
			INTOUT();
			ND_PRINT((ndo, " domain"));
			STROUT(KANAMEMAX);
			ND_TCHECK2(bp[0], sizeof(int32_t));
			i = (int) EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);
			ND_TCHECK2(bp[0], i);
			bp += i;
			ND_PRINT((ndo, " principal"));
			STROUT(KANAMEMAX);
			STROUT(KANAMEMAX);
			break;
		}
		case 4:		/* Set Password */
			ND_PRINT((ndo, " principal"));
			STROUT(KANAMEMAX);
			STROUT(KANAMEMAX);
			ND_PRINT((ndo, " kvno"));
			INTOUT();
			break;
		case 12:	/* Get password */
			ND_PRINT((ndo, " name"));
			STROUT(KANAMEMAX);
			break;
		default:
			;
	}

	return;

trunc:
	ND_PRINT((ndo, " [|kauth]"));
}
