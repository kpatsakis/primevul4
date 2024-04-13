static int brcmf_dev_pno_config(struct brcmf_if *ifp,
				struct cfg80211_sched_scan_request *request)
{
	struct brcmf_pno_param_le pfn_param;
	struct brcmf_pno_macaddr_le pfn_mac;
	s32 err;
	u8 *mac_mask;
	int i;

	memset(&pfn_param, 0, sizeof(pfn_param));
	pfn_param.version = cpu_to_le32(BRCMF_PNO_VERSION);

	/* set extra pno params */
	pfn_param.flags = cpu_to_le16(1 << BRCMF_PNO_ENABLE_ADAPTSCAN_BIT);
	pfn_param.repeat = BRCMF_PNO_REPEAT;
	pfn_param.exp = BRCMF_PNO_FREQ_EXPO_MAX;

	/* set up pno scan fr */
	pfn_param.scan_freq = cpu_to_le32(BRCMF_PNO_TIME);

	err = brcmf_fil_iovar_data_set(ifp, "pfn_set", &pfn_param,
				       sizeof(pfn_param));
	if (err) {
		brcmf_err("pfn_set failed, err=%d\n", err);
		return err;
	}

	/* Find out if mac randomization should be turned on */
	if (!(request->flags & NL80211_SCAN_FLAG_RANDOM_ADDR))
		return 0;

	pfn_mac.version = BRCMF_PFN_MACADDR_CFG_VER;
	pfn_mac.flags = BRCMF_PFN_MAC_OUI_ONLY | BRCMF_PFN_SET_MAC_UNASSOC;

	memcpy(pfn_mac.mac, request->mac_addr, ETH_ALEN);
	mac_mask = request->mac_addr_mask;
	for (i = 0; i < ETH_ALEN; i++) {
		pfn_mac.mac[i] &= mac_mask[i];
		pfn_mac.mac[i] |= get_random_int() & ~(mac_mask[i]);
	}
	/* Clear multi bit */
	pfn_mac.mac[0] &= 0xFE;
	/* Set locally administered */
	pfn_mac.mac[0] |= 0x02;

	err = brcmf_fil_iovar_data_set(ifp, "pfn_macaddr", &pfn_mac,
				       sizeof(pfn_mac));
	if (err)
		brcmf_err("pfn_macaddr failed, err=%d\n", err);

	return err;
}
