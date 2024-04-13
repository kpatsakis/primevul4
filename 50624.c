static s32 brcmf_translate_country_code(struct brcmf_pub *drvr, char alpha2[2],
					struct brcmf_fil_country_le *ccreq)
{
	struct brcmfmac_pd_cc *country_codes;
	struct brcmfmac_pd_cc_entry *cc;
	s32 found_index;
	int i;

	country_codes = drvr->settings->country_codes;
	if (!country_codes) {
		brcmf_dbg(TRACE, "No country codes configured for device\n");
		return -EINVAL;
	}

	if ((alpha2[0] == ccreq->country_abbrev[0]) &&
	    (alpha2[1] == ccreq->country_abbrev[1])) {
		brcmf_dbg(TRACE, "Country code already set\n");
		return -EAGAIN;
	}

	found_index = -1;
	for (i = 0; i < country_codes->table_size; i++) {
		cc = &country_codes->table[i];
		if ((cc->iso3166[0] == '\0') && (found_index == -1))
			found_index = i;
		if ((cc->iso3166[0] == alpha2[0]) &&
		    (cc->iso3166[1] == alpha2[1])) {
			found_index = i;
			break;
		}
	}
	if (found_index == -1) {
		brcmf_dbg(TRACE, "No country code match found\n");
		return -EINVAL;
	}
	memset(ccreq, 0, sizeof(*ccreq));
	ccreq->rev = cpu_to_le32(country_codes->table[found_index].rev);
	memcpy(ccreq->ccode, country_codes->table[found_index].cc,
	       BRCMF_COUNTRY_BUF_SZ);
	ccreq->country_abbrev[0] = alpha2[0];
	ccreq->country_abbrev[1] = alpha2[1];
	ccreq->country_abbrev[2] = 0;

	return 0;
}
