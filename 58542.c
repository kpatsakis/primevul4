brcmf_configure_wpaie(struct brcmf_if *ifp,
		      const struct brcmf_vs_tlv *wpa_ie,
		      bool is_rsn_ie)
{
	u32 auth = 0; /* d11 open authentication */
	u16 count;
	s32 err = 0;
	s32 len;
	u32 i;
	u32 wsec;
	u32 pval = 0;
	u32 gval = 0;
	u32 wpa_auth = 0;
	u32 offset;
	u8 *data;
	u16 rsn_cap;
	u32 wme_bss_disable;
	u32 mfp;

	brcmf_dbg(TRACE, "Enter\n");
	if (wpa_ie == NULL)
		goto exit;

	len = wpa_ie->len + TLV_HDR_LEN;
	data = (u8 *)wpa_ie;
	offset = TLV_HDR_LEN;
	if (!is_rsn_ie)
		offset += VS_IE_FIXED_HDR_LEN;
	else
		offset += WPA_IE_VERSION_LEN;

	/* check for multicast cipher suite */
	if (offset + WPA_IE_MIN_OUI_LEN > len) {
		err = -EINVAL;
		brcmf_err("no multicast cipher suite\n");
		goto exit;
	}

	if (!brcmf_valid_wpa_oui(&data[offset], is_rsn_ie)) {
		err = -EINVAL;
		brcmf_err("ivalid OUI\n");
		goto exit;
	}
	offset += TLV_OUI_LEN;

	/* pick up multicast cipher */
	switch (data[offset]) {
	case WPA_CIPHER_NONE:
		gval = 0;
		break;
	case WPA_CIPHER_WEP_40:
	case WPA_CIPHER_WEP_104:
		gval = WEP_ENABLED;
		break;
	case WPA_CIPHER_TKIP:
		gval = TKIP_ENABLED;
		break;
	case WPA_CIPHER_AES_CCM:
		gval = AES_ENABLED;
		break;
	default:
		err = -EINVAL;
		brcmf_err("Invalid multi cast cipher info\n");
		goto exit;
	}

	offset++;
	/* walk thru unicast cipher list and pick up what we recognize */
	count = data[offset] + (data[offset + 1] << 8);
	offset += WPA_IE_SUITE_COUNT_LEN;
	/* Check for unicast suite(s) */
	if (offset + (WPA_IE_MIN_OUI_LEN * count) > len) {
		err = -EINVAL;
		brcmf_err("no unicast cipher suite\n");
		goto exit;
	}
	for (i = 0; i < count; i++) {
		if (!brcmf_valid_wpa_oui(&data[offset], is_rsn_ie)) {
			err = -EINVAL;
			brcmf_err("ivalid OUI\n");
			goto exit;
		}
		offset += TLV_OUI_LEN;
		switch (data[offset]) {
		case WPA_CIPHER_NONE:
			break;
		case WPA_CIPHER_WEP_40:
		case WPA_CIPHER_WEP_104:
			pval |= WEP_ENABLED;
			break;
		case WPA_CIPHER_TKIP:
			pval |= TKIP_ENABLED;
			break;
		case WPA_CIPHER_AES_CCM:
			pval |= AES_ENABLED;
			break;
		default:
			brcmf_err("Invalid unicast security info\n");
		}
		offset++;
	}
	/* walk thru auth management suite list and pick up what we recognize */
	count = data[offset] + (data[offset + 1] << 8);
	offset += WPA_IE_SUITE_COUNT_LEN;
	/* Check for auth key management suite(s) */
	if (offset + (WPA_IE_MIN_OUI_LEN * count) > len) {
		err = -EINVAL;
		brcmf_err("no auth key mgmt suite\n");
		goto exit;
	}
	for (i = 0; i < count; i++) {
		if (!brcmf_valid_wpa_oui(&data[offset], is_rsn_ie)) {
			err = -EINVAL;
			brcmf_err("ivalid OUI\n");
			goto exit;
		}
		offset += TLV_OUI_LEN;
		switch (data[offset]) {
		case RSN_AKM_NONE:
			brcmf_dbg(TRACE, "RSN_AKM_NONE\n");
			wpa_auth |= WPA_AUTH_NONE;
			break;
		case RSN_AKM_UNSPECIFIED:
			brcmf_dbg(TRACE, "RSN_AKM_UNSPECIFIED\n");
			is_rsn_ie ? (wpa_auth |= WPA2_AUTH_UNSPECIFIED) :
				    (wpa_auth |= WPA_AUTH_UNSPECIFIED);
			break;
		case RSN_AKM_PSK:
			brcmf_dbg(TRACE, "RSN_AKM_PSK\n");
			is_rsn_ie ? (wpa_auth |= WPA2_AUTH_PSK) :
				    (wpa_auth |= WPA_AUTH_PSK);
			break;
		case RSN_AKM_SHA256_PSK:
			brcmf_dbg(TRACE, "RSN_AKM_MFP_PSK\n");
			wpa_auth |= WPA2_AUTH_PSK_SHA256;
			break;
		case RSN_AKM_SHA256_1X:
			brcmf_dbg(TRACE, "RSN_AKM_MFP_1X\n");
			wpa_auth |= WPA2_AUTH_1X_SHA256;
			break;
		default:
			brcmf_err("Invalid key mgmt info\n");
		}
		offset++;
	}

	mfp = BRCMF_MFP_NONE;
	if (is_rsn_ie) {
		wme_bss_disable = 1;
		if ((offset + RSN_CAP_LEN) <= len) {
			rsn_cap = data[offset] + (data[offset + 1] << 8);
			if (rsn_cap & RSN_CAP_PTK_REPLAY_CNTR_MASK)
				wme_bss_disable = 0;
			if (rsn_cap & RSN_CAP_MFPR_MASK) {
				brcmf_dbg(TRACE, "MFP Required\n");
				mfp = BRCMF_MFP_REQUIRED;
				/* Firmware only supports mfp required in
				 * combination with WPA2_AUTH_PSK_SHA256 or
				 * WPA2_AUTH_1X_SHA256.
				 */
				if (!(wpa_auth & (WPA2_AUTH_PSK_SHA256 |
						  WPA2_AUTH_1X_SHA256))) {
					err = -EINVAL;
					goto exit;
				}
				/* Firmware has requirement that WPA2_AUTH_PSK/
				 * WPA2_AUTH_UNSPECIFIED be set, if SHA256 OUI
				 * is to be included in the rsn ie.
				 */
				if (wpa_auth & WPA2_AUTH_PSK_SHA256)
					wpa_auth |= WPA2_AUTH_PSK;
				else if (wpa_auth & WPA2_AUTH_1X_SHA256)
					wpa_auth |= WPA2_AUTH_UNSPECIFIED;
			} else if (rsn_cap & RSN_CAP_MFPC_MASK) {
				brcmf_dbg(TRACE, "MFP Capable\n");
				mfp = BRCMF_MFP_CAPABLE;
			}
		}
		offset += RSN_CAP_LEN;
		/* set wme_bss_disable to sync RSN Capabilities */
		err = brcmf_fil_bsscfg_int_set(ifp, "wme_bss_disable",
					       wme_bss_disable);
		if (err < 0) {
			brcmf_err("wme_bss_disable error %d\n", err);
			goto exit;
		}

		/* Skip PMKID cnt as it is know to be 0 for AP. */
		offset += RSN_PMKID_COUNT_LEN;

		/* See if there is BIP wpa suite left for MFP */
		if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MFP) &&
		    ((offset + WPA_IE_MIN_OUI_LEN) <= len)) {
			err = brcmf_fil_bsscfg_data_set(ifp, "bip",
							&data[offset],
							WPA_IE_MIN_OUI_LEN);
			if (err < 0) {
				brcmf_err("bip error %d\n", err);
				goto exit;
			}
		}
	}
	/* FOR WPS , set SES_OW_ENABLED */
	wsec = (pval | gval | SES_OW_ENABLED);

	/* set auth */
	err = brcmf_fil_bsscfg_int_set(ifp, "auth", auth);
	if (err < 0) {
		brcmf_err("auth error %d\n", err);
		goto exit;
	}
	/* set wsec */
	err = brcmf_fil_bsscfg_int_set(ifp, "wsec", wsec);
	if (err < 0) {
		brcmf_err("wsec error %d\n", err);
		goto exit;
	}
	/* Configure MFP, this needs to go after wsec otherwise the wsec command
	 * will overwrite the values set by MFP
	 */
	if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MFP)) {
		err = brcmf_fil_bsscfg_int_set(ifp, "mfp", mfp);
		if (err < 0) {
			brcmf_err("mfp error %d\n", err);
			goto exit;
		}
	}
	/* set upper-layer auth */
	err = brcmf_fil_bsscfg_int_set(ifp, "wpa_auth", wpa_auth);
	if (err < 0) {
		brcmf_err("wpa_auth error %d\n", err);
		goto exit;
	}

exit:
	return err;
}
