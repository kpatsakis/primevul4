mgmt_header_print(netdissect_options *ndo, const u_char *p)
{
	const struct mgmt_header_t *hp = (const struct mgmt_header_t *) p;

	ND_PRINT((ndo, "BSSID:%s DA:%s SA:%s ",
	    etheraddr_string(ndo, (hp)->bssid), etheraddr_string(ndo, (hp)->da),
	    etheraddr_string(ndo, (hp)->sa)));
}
