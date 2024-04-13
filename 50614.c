static void brcmf_set_join_pref(struct brcmf_if *ifp,
				struct cfg80211_bss_selection *bss_select)
{
	struct brcmf_join_pref_params join_pref_params[2];
	enum nl80211_band band;
	int err, i = 0;

	join_pref_params[i].len = 2;
	join_pref_params[i].rssi_gain = 0;

	if (bss_select->behaviour != NL80211_BSS_SELECT_ATTR_BAND_PREF)
		brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_ASSOC_PREFER, WLC_BAND_AUTO);

	switch (bss_select->behaviour) {
	case __NL80211_BSS_SELECT_ATTR_INVALID:
		brcmf_c_set_joinpref_default(ifp);
		return;
	case NL80211_BSS_SELECT_ATTR_BAND_PREF:
		join_pref_params[i].type = BRCMF_JOIN_PREF_BAND;
		band = bss_select->param.band_pref;
		join_pref_params[i].band = nl80211_band_to_fwil(band);
		i++;
		break;
	case NL80211_BSS_SELECT_ATTR_RSSI_ADJUST:
		join_pref_params[i].type = BRCMF_JOIN_PREF_RSSI_DELTA;
		band = bss_select->param.adjust.band;
		join_pref_params[i].band = nl80211_band_to_fwil(band);
		join_pref_params[i].rssi_gain = bss_select->param.adjust.delta;
		i++;
		break;
	case NL80211_BSS_SELECT_ATTR_RSSI:
	default:
		break;
	}
	join_pref_params[i].type = BRCMF_JOIN_PREF_RSSI;
	join_pref_params[i].len = 2;
	join_pref_params[i].rssi_gain = 0;
	join_pref_params[i].band = 0;
	err = brcmf_fil_iovar_data_set(ifp, "join_pref", join_pref_params,
				       sizeof(join_pref_params));
	if (err)
		brcmf_err("Set join_pref error (%d)\n", err);
}
