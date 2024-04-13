ospf6_print_lsaprefix(netdissect_options *ndo,
                      const uint8_t *tptr, u_int lsa_length)
{
	const struct lsa6_prefix *lsapp = (const struct lsa6_prefix *)tptr;
	u_int wordlen;
	struct in6_addr prefix;

	if (lsa_length < sizeof (*lsapp) - IPV6_ADDR_LEN_BYTES)
		goto trunc;
	lsa_length -= sizeof (*lsapp) - IPV6_ADDR_LEN_BYTES;
	ND_TCHECK2(*lsapp, sizeof (*lsapp) - IPV6_ADDR_LEN_BYTES);
	wordlen = (lsapp->lsa_p_len + 31) / 32;
	if (wordlen * 4 > sizeof(struct in6_addr)) {
		ND_PRINT((ndo, " bogus prefixlen /%d", lsapp->lsa_p_len));
		goto trunc;
	}
	if (lsa_length < wordlen * 4)
		goto trunc;
	lsa_length -= wordlen * 4;
	ND_TCHECK2(lsapp->lsa_p_prefix, wordlen * 4);
	memset(&prefix, 0, sizeof(prefix));
	memcpy(&prefix, lsapp->lsa_p_prefix, wordlen * 4);
	ND_PRINT((ndo, "\n\t\t%s/%d", ip6addr_string(ndo, &prefix),
		lsapp->lsa_p_len));
        if (lsapp->lsa_p_opt) {
            ND_PRINT((ndo, ", Options [%s]",
                   bittok2str(ospf6_lsa_prefix_option_values,
                              "none", lsapp->lsa_p_opt)));
        }
        ND_PRINT((ndo, ", metric %u", EXTRACT_16BITS(&lsapp->lsa_p_metric)));
	return sizeof(*lsapp) - IPV6_ADDR_LEN_BYTES + wordlen * 4;

trunc:
	return -1;
}
