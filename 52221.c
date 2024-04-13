bid_keycmp(const char *p, const char *key, ssize_t len)
{
	int match_len = 0;

	while (len > 0 && *p && *key) {
		if (*p == *key) {
			--len;
			++p;
			++key;
			++match_len;
			continue;
		}
		return (0);/* Not match */
	}
	if (*key != '\0')
		return (0);/* Not match */

	/* A following character should be specified characters */
	if (p[0] == '=' || p[0] == ' ' || p[0] == '\t' ||
	    p[0] == '\n' || p[0] == '\r' ||
	   (p[0] == '\\' && (p[1] == '\n' || p[1] == '\r')))
		return (match_len);
	return (0);/* Not match */
}
