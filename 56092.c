cb_print(netdissect_options *ndo,
         register const u_char *bp, int length)
{
	int cb_op;
	unsigned long i;

	if (length <= (int)sizeof(struct rx_header))
		return;

	if (ndo->ndo_snapend - bp + 1 <= (int)(sizeof(struct rx_header) + sizeof(int32_t))) {
		goto trunc;
	}

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from fsint/afscbint.xg
	 */

	cb_op = EXTRACT_32BITS(bp + sizeof(struct rx_header));

	ND_PRINT((ndo, " cb call %s", tok2str(cb_req, "op#%d", cb_op)));

	bp += sizeof(struct rx_header) + 4;

	/*
	 * Print out the afs call we're invoking.  The table used here was
	 * gleaned from fsint/afscbint.xg
	 */

	switch (cb_op) {
		case 204:		/* Callback */
		{
			unsigned long j, t;
			ND_TCHECK2(bp[0], 4);
			j = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);

			for (i = 0; i < j; i++) {
				FIDOUT();
				if (i != j - 1)
					ND_PRINT((ndo, ","));
			}

			if (j == 0)
				ND_PRINT((ndo, " <none!>"));

			j = EXTRACT_32BITS(bp);
			bp += sizeof(int32_t);

			if (j != 0)
				ND_PRINT((ndo, ";"));

			for (i = 0; i < j; i++) {
				ND_PRINT((ndo, " ver"));
				INTOUT();
				ND_PRINT((ndo, " expires"));
				DATEOUT();
				ND_TCHECK2(bp[0], 4);
				t = EXTRACT_32BITS(bp);
				bp += sizeof(int32_t);
				tok2str(cb_types, "type %d", t);
			}
		}
		case 214: {
			ND_PRINT((ndo, " afsuuid"));
			AFSUUIDOUT();
			break;
		}
		default:
			;
	}

	return;

trunc:
	ND_PRINT((ndo, " [|cb]"));
}
