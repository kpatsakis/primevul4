txtproto_print(netdissect_options *ndo, const u_char *pptr, u_int len,
    const char *protoname, const char **cmds, u_int flags)
{
	u_int idx, eol;
	u_char token[MAX_TOKEN+1];
	const char *cmd;
	int is_reqresp = 0;
	const char *pnp;

	if (cmds != NULL) {
		/*
		 * This protocol has more than just request and
		 * response lines; see whether this looks like a
		 * request or response.
		 */
		idx = fetch_token(ndo, pptr, 0, len, token, sizeof(token));
		if (idx != 0) {
			/* Is this a valid request name? */
			while ((cmd = *cmds++) != NULL) {
				if (ascii_strcasecmp((const char *)token, cmd) == 0) {
					/* Yes. */
					is_reqresp = 1;
					break;
				}
			}

			/*
			 * No - is this a valid response code (3 digits)?
			 *
			 * Is this token the response code, or is the next
			 * token the response code?
			 */
			if (flags & RESP_CODE_SECOND_TOKEN) {
				/*
				 * Next token - get it.
				 */
				idx = fetch_token(ndo, pptr, idx, len, token,
				    sizeof(token));
			}
			if (idx != 0) {
				if (isdigit(token[0]) && isdigit(token[1]) &&
				    isdigit(token[2]) && token[3] == '\0') {
					/* Yes. */
					is_reqresp = 1;
				}
			}
		}
	} else {
		/*
		 * This protocol has only request and response lines
		 * (e.g., FTP, where all the data goes over a
		 * different connection); assume the payload is
		 * a request or response.
		 */
		is_reqresp = 1;
	}

	/* Capitalize the protocol name */
	for (pnp = protoname; *pnp != '\0'; pnp++)
		ND_PRINT((ndo, "%c", toupper((u_char)*pnp)));

	if (is_reqresp) {
		/*
		 * In non-verbose mode, just print the protocol, followed
		 * by the first line as the request or response info.
		 *
		 * In verbose mode, print lines as text until we run out
		 * of characters or see something that's not a
		 * printable-ASCII line.
		 */
		if (ndo->ndo_vflag) {
			/*
			 * We're going to print all the text lines in the
			 * request or response; just print the length
			 * on the first line of the output.
			 */
			ND_PRINT((ndo, ", length: %u", len));
			for (idx = 0;
			    idx < len && (eol = print_txt_line(ndo, protoname, "\n\t", pptr, idx, len)) != 0;
			    idx = eol)
				;
		} else {
			/*
			 * Just print the first text line.
			 */
			print_txt_line(ndo, protoname, ": ", pptr, 0, len);
		}
	}
}
