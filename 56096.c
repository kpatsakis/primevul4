prot_print(netdissect_options *ndo,
           register const u_char *bp, int length)
{
	unsigned long i;
	int pt_op;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from ptserver/ptint.xg
	 */

	pt_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " pt"));

	if (is_ubik(pt_op)) {
		ubik_print(ndo, bp);
		return;
	}

	ND_PRINT((ndo, " call %s", tok2str(pt_req, "op#%d", pt_op)));

	/*
	 * Decode some of the arguments to the PT calls
	 */

	bp += sizeof(struct rx_header) + 4;

	switch (pt_op) {
		case 500:	/* I New User */
			STROUT(PRNAMEMAX);
			ND_PRINT((ndo, " id"));
			INTOUT();
			ND_PRINT((ndo, " oldid"));
			INTOUT();
			break;
		case 501:	/* Where is it */
		case 506:	/* Delete */
		case 508:	/* Get CPS */
		case 512:	/* List entry */
		case 514:	/* List elements */
		case 517:	/* List owned */
		case 518:	/* Get CPS2 */
		case 519:	/* Get host CPS */
		case 530:	/* List super groups */
			ND_PRINT((ndo, " id"));
			INTOUT();
			break;
		case 502:	/* Dump entry */
			ND_PRINT((ndo, " pos"));
			INTOUT();
			break;
		case 503:	/* Add to group */
		case 507:	/* Remove from group */
		case 515:	/* Is a member of? */
			ND_PRINT((ndo, " uid"));
			INTOUT();
			ND_PRINT((ndo, " gid"));
			INTOUT();
			break;
		case 504:	/* Name to ID */
		{
			unsigned long j;
			ND_TCHECK2(bp[0], 4);
			j = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);

			/*
			 * Who designed this chicken-shit protocol?
			 *
			 * Each character is stored as a 32-bit
			 * integer!
			 */

			for (i = 0; i < j; i++) {
				VECOUT(PRNAMEMAX);
			}
			if (j == 0)
				ND_PRINT((ndo, " <none!>"));
		}
			break;
		case 505:	/* Id to name */
		{
			unsigned long j;
			ND_PRINT((ndo, " ids:"));
			ND_TCHECK2(bp[0], 4);
			i = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);
			for (j = 0; j < i; j++)
				INTOUT();
			if (j == 0)
				ND_PRINT((ndo, " <none!>"));
		}
			break;
		case 509:	/* New entry */
			STROUT(PRNAMEMAX);
			ND_PRINT((ndo, " flag"));
			INTOUT();
			ND_PRINT((ndo, " oid"));
			INTOUT();
			break;
		case 511:	/* Set max */
			ND_PRINT((ndo, " id"));
			INTOUT();
			ND_PRINT((ndo, " gflag"));
			INTOUT();
			break;
		case 513:	/* Change entry */
			ND_PRINT((ndo, " id"));
			INTOUT();
			STROUT(PRNAMEMAX);
			ND_PRINT((ndo, " oldid"));
			INTOUT();
			ND_PRINT((ndo, " newid"));
			INTOUT();
			break;
		case 520:	/* Update entry */
			ND_PRINT((ndo, " id"));
			INTOUT();
			STROUT(PRNAMEMAX);
			break;
		default:
			;
	}


	return;

trunc:
	ND_PRINT((ndo, " [|pt]"));
}
