q933_print(netdissect_options *ndo,
           const u_char *p, u_int length)
{
	u_int olen;
	u_int call_ref_length, i;
	uint8_t call_ref[15];	/* maximum length - length field is 4 bits */
	u_int msgtype;
	u_int iecode;
	u_int ielength;
	u_int codeset = 0;
	u_int is_ansi = 0;
	u_int ie_is_known;
	u_int non_locking_shift;
	u_int unshift_codeset;

	ND_PRINT((ndo, "%s", ndo->ndo_eflag ? "" : "Q.933"));

	if (length == 0 || !ND_TTEST(*p)) {
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, ", "));
		ND_PRINT((ndo, "length %u", length));
		goto trunc;
	}

	/*
	 * Get the length of the call reference value.
	 */
	olen = length; /* preserve the original length for display */
	call_ref_length = (*p) & 0x0f;
	p++;
	length--;

	/*
	 * Get the call reference value.
	 */
	for (i = 0; i < call_ref_length; i++) {
		if (length == 0 || !ND_TTEST(*p)) {
			if (!ndo->ndo_eflag)
				ND_PRINT((ndo, ", "));
			ND_PRINT((ndo, "length %u", olen));
			goto trunc;
		}
		call_ref[i] = *p;
		p++;
		length--;
	}

	/*
	 * Get the message type.
	 */
	if (length == 0 || !ND_TTEST(*p)) {
		if (!ndo->ndo_eflag)
			ND_PRINT((ndo, ", "));
		ND_PRINT((ndo, "length %u", olen));
		goto trunc;
	}
	msgtype = *p;
	p++;
	length--;

	/*
	 * Peek ahead to see if we start with a shift.
	 */
	non_locking_shift = 0;
	unshift_codeset = codeset;
	if (length != 0) {
		if (!ND_TTEST(*p)) {
			if (!ndo->ndo_eflag)
				ND_PRINT((ndo, ", "));
			ND_PRINT((ndo, "length %u", olen));
			goto trunc;
		}
		iecode = *p;
		if (IE_IS_SHIFT(iecode)) {
			/*
			 * It's a shift.  Skip over it.
			 */
			p++;
			length--;

			/*
			 * Get the codeset.
			 */
			codeset = IE_SHIFT_CODESET(iecode);

			/*
			 * If it's a locking shift to codeset 5,
			 * mark this as ANSI.  (XXX - 5 is actually
			 * for national variants in general, not
			 * the US variant in particular, but maybe
			 * this is more American exceptionalism. :-))
			 */
			if (IE_SHIFT_IS_LOCKING(iecode)) {
				/*
				 * It's a locking shift.
				 */
				if (codeset == 5) {
					/*
					 * It's a locking shift to
					 * codeset 5, so this is
					 * T1.617 Annex D.
					 */
					is_ansi = 1;
				}
			} else {
				/*
				 * It's a non-locking shift.
				 * Remember the current codeset, so we
				 * can revert to it after the next IE.
				 */
				non_locking_shift = 1;
				unshift_codeset = 0;
			}
		}
	}

	/* printing out header part */
	if (!ndo->ndo_eflag)
		ND_PRINT((ndo, ", "));
	ND_PRINT((ndo, "%s, codeset %u", is_ansi ? "ANSI" : "CCITT", codeset));

	if (call_ref_length != 0) {
		ND_TCHECK(p[0]);
		if (call_ref_length > 1 || p[0] != 0) {
			/*
			 * Not a dummy call reference.
			 */
			ND_PRINT((ndo, ", Call Ref: 0x"));
			for (i = 0; i < call_ref_length; i++)
				ND_PRINT((ndo, "%02x", call_ref[i]));
		}
	}
	if (ndo->ndo_vflag) {
		ND_PRINT((ndo, ", %s (0x%02x), length %u",
		   tok2str(fr_q933_msg_values,
			"unknown message", msgtype),
		   msgtype,
		   olen));
	} else {
		ND_PRINT((ndo, ", %s",
		       tok2str(fr_q933_msg_values,
			       "unknown message 0x%02x", msgtype)));
	}

	/* Loop through the rest of the IEs */
	while (length != 0) {
		/*
		 * What's the state of any non-locking shifts?
		 */
		if (non_locking_shift == 1) {
			/*
			 * There's a non-locking shift in effect for
			 * this IE.  Count it, so we reset the codeset
			 * before the next IE.
			 */
			non_locking_shift = 2;
		} else if (non_locking_shift == 2) {
			/*
			 * Unshift.
			 */
			codeset = unshift_codeset;
			non_locking_shift = 0;
		}

		/*
		 * Get the first octet of the IE.
		 */
		if (!ND_TTEST(*p)) {
			if (!ndo->ndo_vflag) {
				ND_PRINT((ndo, ", length %u", olen));
			}
			goto trunc;
		}
		iecode = *p;
		p++;
		length--;

		/* Single-octet IE? */
		if (IE_IS_SINGLE_OCTET(iecode)) {
			/*
			 * Yes.  Is it a shift?
			 */
			if (IE_IS_SHIFT(iecode)) {
				/*
				 * Yes.  Is it locking?
				 */
				if (IE_SHIFT_IS_LOCKING(iecode)) {
					/*
					 * Yes.
					 */
					non_locking_shift = 0;
				} else {
					/*
					 * No.  Remember the current
					 * codeset, so we can revert
					 * to it after the next IE.
					 */
					non_locking_shift = 1;
					unshift_codeset = codeset;
				}

				/*
				 * Get the codeset.
				 */
				codeset = IE_SHIFT_CODESET(iecode);
			}
		} else {
			/*
			 * No.  Get the IE length.
			 */
			if (length == 0 || !ND_TTEST(*p)) {
				if (!ndo->ndo_vflag) {
					ND_PRINT((ndo, ", length %u", olen));
				}
				goto trunc;
			}
			ielength = *p;
			p++;
			length--;

			/* lets do the full IE parsing only in verbose mode
			 * however some IEs (DLCI Status, Link Verify)
			 * are also interesting in non-verbose mode */
			if (ndo->ndo_vflag) {
				ND_PRINT((ndo, "\n\t%s IE (0x%02x), length %u: ",
				    tok2str(fr_q933_ie_codesets[codeset],
					"unknown", iecode),
				    iecode,
				    ielength));
			}

			/* sanity checks */
			if (iecode == 0 || ielength == 0) {
				return;
			}
			if (length < ielength || !ND_TTEST2(*p, ielength)) {
				if (!ndo->ndo_vflag) {
					ND_PRINT((ndo, ", length %u", olen));
				}
				goto trunc;
			}

			ie_is_known = 0;
			if (fr_q933_print_ie_codeset[codeset] != NULL) {
				ie_is_known = fr_q933_print_ie_codeset[codeset](ndo, iecode, ielength, p);
			}

			if (ie_is_known) {
				/*
				 * Known IE; do we want to see a hexdump
				 * of it?
				 */
				if (ndo->ndo_vflag > 1) {
					/* Yes. */
					print_unknown_data(ndo, p, "\n\t  ", ielength);
				}
			} else {
				/*
				 * Unknown IE; if we're printing verbosely,
				 * print its content in hex.
				 */
				if (ndo->ndo_vflag >= 1) {
					print_unknown_data(ndo, p, "\n\t", ielength);
				}
			}

			length -= ielength;
			p += ielength;
		}
	}
	if (!ndo->ndo_vflag) {
	    ND_PRINT((ndo, ", length %u", olen));
	}
	return;

trunc:
	ND_PRINT((ndo, "[|q.933]"));
}
