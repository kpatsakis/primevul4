s32 brcmf_vif_clear_mgmt_ies(struct brcmf_cfg80211_vif *vif)
{
	s32 pktflags[] = {
		BRCMF_VNDR_IE_PRBREQ_FLAG,
		BRCMF_VNDR_IE_PRBRSP_FLAG,
		BRCMF_VNDR_IE_BEACON_FLAG
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(pktflags); i++)
		brcmf_vif_set_mgmt_ie(vif, pktflags[i], NULL, 0);

	memset(&vif->saved_ie, 0, sizeof(vif->saved_ie));
	return 0;
}
