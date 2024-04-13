fetch_token(netdissect_options *ndo, const u_char *pptr, u_int idx, u_int len,
    u_char *tbuf, size_t tbuflen)
{
	size_t toklen = 0;

	for (; idx < len; idx++) {
		if (!ND_TTEST(*(pptr + idx))) {
			/* ran past end of captured data */
			return (0);
		}
		if (!isascii(*(pptr + idx))) {
			/* not an ASCII character */
			return (0);
		}
		if (isspace(*(pptr + idx))) {
			/* end of token */
			break;
		}
		if (!isprint(*(pptr + idx))) {
			/* not part of a command token or response code */
			return (0);
		}
		if (toklen + 2 > tbuflen) {
			/* no room for this character and terminating '\0' */
			return (0);
		}
		tbuf[toklen] = *(pptr + idx);
		toklen++;
	}
	if (toklen == 0) {
		/* no token */
		return (0);
	}
	tbuf[toklen] = '\0';

	/*
	 * Skip past any white space after the token, until we see
	 * an end-of-line (CR or LF).
	 */
	for (; idx < len; idx++) {
		if (!ND_TTEST(*(pptr + idx))) {
			/* ran past end of captured data */
			break;
		}
		if (*(pptr + idx) == '\r' || *(pptr + idx) == '\n') {
			/* end of line */
			break;
		}
		if (!isascii(*(pptr + idx)) || !isprint(*(pptr + idx))) {
			/* not a printable ASCII character */
			break;
		}
		if (!isspace(*(pptr + idx))) {
			/* beginning of next token */
			break;
		}
	}
	return (idx);
}
