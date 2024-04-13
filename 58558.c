static int brcmf_set_pmk(struct brcmf_if *ifp, const u8 *pmk_data, u16 pmk_len)
{
	struct brcmf_wsec_pmk_le pmk;
	int i, err;

	/* convert to firmware key format */
	pmk.key_len = cpu_to_le16(pmk_len << 1);
	pmk.flags = cpu_to_le16(BRCMF_WSEC_PASSPHRASE);
	for (i = 0; i < pmk_len; i++)
		snprintf(&pmk.key[2 * i], 3, "%02x", pmk_data[i]);

	/* store psk in firmware */
	err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_WSEC_PMK,
				     &pmk, sizeof(pmk));
	if (err < 0)
		brcmf_err("failed to change PSK in firmware (len=%u)\n",
			  pmk_len);

	return err;
}
