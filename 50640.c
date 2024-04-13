static bool check_vif_up(struct brcmf_cfg80211_vif *vif)
{
	if (!test_bit(BRCMF_VIF_STATUS_READY, &vif->sme_state)) {
		brcmf_dbg(INFO, "device is not ready : status (%lu)\n",
			  vif->sme_state);
		return false;
	}
	return true;
}
