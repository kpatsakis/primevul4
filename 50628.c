brcmf_update_pmklist(struct brcmf_cfg80211_info *cfg, struct brcmf_if *ifp)
{
	struct brcmf_pmk_list_le *pmk_list;
	int i;
	u32 npmk;
	s32 err;

	pmk_list = &cfg->pmk_list;
	npmk = le32_to_cpu(pmk_list->npmk);

	brcmf_dbg(CONN, "No of elements %d\n", npmk);
	for (i = 0; i < npmk; i++)
		brcmf_dbg(CONN, "PMK[%d]: %pM\n", i, &pmk_list->pmk[i].bssid);

	err = brcmf_fil_iovar_data_set(ifp, "pmkid_info", pmk_list,
				       sizeof(*pmk_list));

	return err;
}
