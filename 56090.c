bos_print(netdissect_options *ndo,
          register const u_char *bp, int length)
{
	int bos_op;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from bozo/bosint.xg
	 */

	bos_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " bos call %s", tok2str(bos_req, "op#%d", bos_op)));

	/*
	 * Decode some of the arguments to the BOS calls
	 */

	bp += sizeof(struct rx_header) + 4;

	switch (bos_op) {
		case 80:	/* Create B node */
			ND_PRINT((ndo, " type"));
			STROUT(BOSNAMEMAX);
			ND_PRINT((ndo, " instance"));
			STROUT(BOSNAMEMAX);
			break;
		case 81:	/* Delete B node */
		case 83:	/* Get status */
		case 85:	/* Get instance info */
		case 87:	/* Add super user */
		case 88:	/* Delete super user */
		case 93:	/* Set cell name */
		case 96:	/* Add cell host */
		case 97:	/* Delete cell host */
		case 104:	/* Restart */
		case 106:	/* Uninstall */
		case 108:	/* Exec */
		case 112:	/* Getlog */
		case 114:	/* Get instance strings */
			STROUT(BOSNAMEMAX);
			break;
		case 82:	/* Set status */
		case 98:	/* Set T status */
			STROUT(BOSNAMEMAX);
			ND_PRINT((ndo, " status"));
			INTOUT();
			break;
		case 86:	/* Get instance parm */
			STROUT(BOSNAMEMAX);
			ND_PRINT((ndo, " num"));
			INTOUT();
			break;
		case 84:	/* Enumerate instance */
		case 89:	/* List super users */
		case 90:	/* List keys */
		case 91:	/* Add key */
		case 92:	/* Delete key */
		case 95:	/* Get cell host */
			INTOUT();
			break;
		case 105:	/* Install */
			STROUT(BOSNAMEMAX);
			ND_PRINT((ndo, " size"));
			INTOUT();
			ND_PRINT((ndo, " flags"));
			INTOUT();
			ND_PRINT((ndo, " date"));
			INTOUT();
			break;
		default:
			;
	}

	return;

trunc:
	ND_PRINT((ndo, " [|bos]"));
}
