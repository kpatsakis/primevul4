static void brcmf_get_bwcap(struct brcmf_if *ifp, u32 bw_cap[])
{
	u32 band, mimo_bwcap;
	int err;

	band = WLC_BAND_2G;
	err = brcmf_fil_iovar_int_get(ifp, "bw_cap", &band);
	if (!err) {
		bw_cap[NL80211_BAND_2GHZ] = band;
		band = WLC_BAND_5G;
		err = brcmf_fil_iovar_int_get(ifp, "bw_cap", &band);
		if (!err) {
			bw_cap[NL80211_BAND_5GHZ] = band;
			return;
		}
		WARN_ON(1);
		return;
	}
	brcmf_dbg(INFO, "fallback to mimo_bw_cap info\n");
	mimo_bwcap = 0;
	err = brcmf_fil_iovar_int_get(ifp, "mimo_bw_cap", &mimo_bwcap);
	if (err)
		/* assume 20MHz if firmware does not give a clue */
		mimo_bwcap = WLC_N_BW_20ALL;

	switch (mimo_bwcap) {
	case WLC_N_BW_40ALL:
		bw_cap[NL80211_BAND_2GHZ] |= WLC_BW_40MHZ_BIT;
		/* fall-thru */
	case WLC_N_BW_20IN2G_40IN5G:
		bw_cap[NL80211_BAND_5GHZ] |= WLC_BW_40MHZ_BIT;
		/* fall-thru */
	case WLC_N_BW_20ALL:
		bw_cap[NL80211_BAND_2GHZ] |= WLC_BW_20MHZ_BIT;
		bw_cap[NL80211_BAND_5GHZ] |= WLC_BW_20MHZ_BIT;
		break;
	default:
		brcmf_err("invalid mimo_bw_cap value\n");
	}
}
