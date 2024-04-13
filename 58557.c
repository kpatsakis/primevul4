brcmf_set_key_mgmt(struct net_device *ndev, struct cfg80211_connect_params *sme)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_cfg80211_profile *profile = &ifp->vif->profile;
	s32 val;
	s32 err;
	const struct brcmf_tlv *rsn_ie;
	const u8 *ie;
	u32 ie_len;
	u32 offset;
	u16 rsn_cap;
	u32 mfp;
	u16 count;

	profile->use_fwsup = BRCMF_PROFILE_FWSUP_NONE;

	if (!sme->crypto.n_akm_suites)
		return 0;

	err = brcmf_fil_bsscfg_int_get(netdev_priv(ndev), "wpa_auth", &val);
	if (err) {
		brcmf_err("could not get wpa_auth (%d)\n", err);
		return err;
	}
	if (val & (WPA_AUTH_PSK | WPA_AUTH_UNSPECIFIED)) {
		switch (sme->crypto.akm_suites[0]) {
		case WLAN_AKM_SUITE_8021X:
			val = WPA_AUTH_UNSPECIFIED;
			if (sme->want_1x)
				profile->use_fwsup = BRCMF_PROFILE_FWSUP_1X;
			break;
		case WLAN_AKM_SUITE_PSK:
			val = WPA_AUTH_PSK;
			break;
		default:
			brcmf_err("invalid cipher group (%d)\n",
				  sme->crypto.cipher_group);
			return -EINVAL;
		}
	} else if (val & (WPA2_AUTH_PSK | WPA2_AUTH_UNSPECIFIED)) {
		switch (sme->crypto.akm_suites[0]) {
		case WLAN_AKM_SUITE_8021X:
			val = WPA2_AUTH_UNSPECIFIED;
			if (sme->want_1x)
				profile->use_fwsup = BRCMF_PROFILE_FWSUP_1X;
			break;
		case WLAN_AKM_SUITE_8021X_SHA256:
			val = WPA2_AUTH_1X_SHA256;
			if (sme->want_1x)
				profile->use_fwsup = BRCMF_PROFILE_FWSUP_1X;
			break;
		case WLAN_AKM_SUITE_PSK_SHA256:
			val = WPA2_AUTH_PSK_SHA256;
			break;
		case WLAN_AKM_SUITE_PSK:
			val = WPA2_AUTH_PSK;
			break;
		default:
			brcmf_err("invalid cipher group (%d)\n",
				  sme->crypto.cipher_group);
			return -EINVAL;
		}
	}

	if (profile->use_fwsup == BRCMF_PROFILE_FWSUP_1X)
		brcmf_dbg(INFO, "using 1X offload\n");

	if (!brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MFP))
		goto skip_mfp_config;
	/* The MFP mode (1 or 2) needs to be determined, parse IEs. The
	 * IE will not be verified, just a quick search for MFP config
	 */
	rsn_ie = brcmf_parse_tlvs((const u8 *)sme->ie, sme->ie_len,
				  WLAN_EID_RSN);
	if (!rsn_ie)
		goto skip_mfp_config;
	ie = (const u8 *)rsn_ie;
	ie_len = rsn_ie->len + TLV_HDR_LEN;
	/* Skip unicast suite */
	offset = TLV_HDR_LEN + WPA_IE_VERSION_LEN + WPA_IE_MIN_OUI_LEN;
	if (offset + WPA_IE_SUITE_COUNT_LEN >= ie_len)
		goto skip_mfp_config;
	/* Skip multicast suite */
	count = ie[offset] + (ie[offset + 1] << 8);
	offset += WPA_IE_SUITE_COUNT_LEN + (count * WPA_IE_MIN_OUI_LEN);
	if (offset + WPA_IE_SUITE_COUNT_LEN >= ie_len)
		goto skip_mfp_config;
	/* Skip auth key management suite(s) */
	count = ie[offset] + (ie[offset + 1] << 8);
	offset += WPA_IE_SUITE_COUNT_LEN + (count * WPA_IE_MIN_OUI_LEN);
	if (offset + WPA_IE_SUITE_COUNT_LEN > ie_len)
		goto skip_mfp_config;
	/* Ready to read capabilities */
	mfp = BRCMF_MFP_NONE;
	rsn_cap = ie[offset] + (ie[offset + 1] << 8);
	if (rsn_cap & RSN_CAP_MFPR_MASK)
		mfp = BRCMF_MFP_REQUIRED;
	else if (rsn_cap & RSN_CAP_MFPC_MASK)
		mfp = BRCMF_MFP_CAPABLE;
	brcmf_fil_bsscfg_int_set(netdev_priv(ndev), "mfp", mfp);

skip_mfp_config:
	brcmf_dbg(CONN, "setting wpa_auth to %d\n", val);
	err = brcmf_fil_bsscfg_int_set(netdev_priv(ndev), "wpa_auth", val);
	if (err) {
		brcmf_err("could not set wpa_auth (%d)\n", err);
		return err;
	}

	return err;
}
