wep_print(netdissect_options *ndo,
          const u_char *p)
{
	uint32_t iv;

	if (!ND_TTEST2(*p, IEEE802_11_IV_LEN + IEEE802_11_KID_LEN))
		return 0;
	iv = EXTRACT_LE_32BITS(p);

	ND_PRINT((ndo, " IV:%3x Pad %x KeyID %x", IV_IV(iv), IV_PAD(iv),
	    IV_KEYID(iv)));

	return 1;
}
