decode_clnp_prefix(netdissect_options *ndo,
                   const u_char *pptr, char *buf, u_int buflen)
{
        uint8_t addr[19];
	u_int plen;

	ND_TCHECK(pptr[0]);
	plen = pptr[0]; /* get prefix length */

	if (152 < plen)
		return -1;

	memset(&addr, 0, sizeof(addr));
	ND_TCHECK2(pptr[4], (plen + 7) / 8);
	memcpy(&addr, &pptr[4], (plen + 7) / 8);
	if (plen % 8) {
		addr[(plen + 7) / 8 - 1] &=
			((0xff00 >> (plen % 8)) & 0xff);
	}
	snprintf(buf, buflen, "%s/%d",
                 isonsap_string(ndo, addr,(plen + 7) / 8),
                 plen);

	return 1 + (plen + 7) / 8;

trunc:
	return -2;
}
