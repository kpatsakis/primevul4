static int brcmf_setup_ifmodes(struct wiphy *wiphy, struct brcmf_if *ifp)
{
	struct ieee80211_iface_combination *combo = NULL;
	struct ieee80211_iface_limit *c0_limits = NULL;
	struct ieee80211_iface_limit *p2p_limits = NULL;
	struct ieee80211_iface_limit *mbss_limits = NULL;
	bool mbss, p2p;
	int i, c, n_combos;

	mbss = brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MBSS);
	p2p = brcmf_feat_is_enabled(ifp, BRCMF_FEAT_P2P);

	n_combos = 1 + !!p2p + !!mbss;
	combo = kcalloc(n_combos, sizeof(*combo), GFP_KERNEL);
	if (!combo)
		goto err;

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
				 BIT(NL80211_IFTYPE_ADHOC) |
				 BIT(NL80211_IFTYPE_AP);

	c = 0;
	i = 0;
	c0_limits = kcalloc(p2p ? 3 : 2, sizeof(*c0_limits), GFP_KERNEL);
	if (!c0_limits)
		goto err;
	c0_limits[i].max = 1;
	c0_limits[i++].types = BIT(NL80211_IFTYPE_STATION);
	if (p2p) {
		if (brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MCHAN))
			combo[c].num_different_channels = 2;
		wiphy->interface_modes |= BIT(NL80211_IFTYPE_P2P_CLIENT) |
					  BIT(NL80211_IFTYPE_P2P_GO) |
					  BIT(NL80211_IFTYPE_P2P_DEVICE);
		c0_limits[i].max = 1;
		c0_limits[i++].types = BIT(NL80211_IFTYPE_P2P_DEVICE);
		c0_limits[i].max = 1;
		c0_limits[i++].types = BIT(NL80211_IFTYPE_P2P_CLIENT) |
				       BIT(NL80211_IFTYPE_P2P_GO);
	} else {
		c0_limits[i].max = 1;
		c0_limits[i++].types = BIT(NL80211_IFTYPE_AP);
	}
	combo[c].num_different_channels = 1;
	combo[c].max_interfaces = i;
	combo[c].n_limits = i;
	combo[c].limits = c0_limits;

	if (p2p) {
		c++;
		i = 0;
		p2p_limits = kcalloc(4, sizeof(*p2p_limits), GFP_KERNEL);
		if (!p2p_limits)
			goto err;
		p2p_limits[i].max = 1;
		p2p_limits[i++].types = BIT(NL80211_IFTYPE_STATION);
		p2p_limits[i].max = 1;
		p2p_limits[i++].types = BIT(NL80211_IFTYPE_AP);
		p2p_limits[i].max = 1;
		p2p_limits[i++].types = BIT(NL80211_IFTYPE_P2P_CLIENT);
		p2p_limits[i].max = 1;
		p2p_limits[i++].types = BIT(NL80211_IFTYPE_P2P_DEVICE);
		combo[c].num_different_channels = 1;
		combo[c].max_interfaces = i;
		combo[c].n_limits = i;
		combo[c].limits = p2p_limits;
	}

	if (mbss) {
		c++;
		i = 0;
		mbss_limits = kcalloc(1, sizeof(*mbss_limits), GFP_KERNEL);
		if (!mbss_limits)
			goto err;
		mbss_limits[i].max = 4;
		mbss_limits[i++].types = BIT(NL80211_IFTYPE_AP);
		combo[c].beacon_int_infra_match = true;
		combo[c].num_different_channels = 1;
		combo[c].max_interfaces = 4;
		combo[c].n_limits = i;
		combo[c].limits = mbss_limits;
	}

	wiphy->n_iface_combinations = n_combos;
	wiphy->iface_combinations = combo;
	return 0;

err:
	kfree(c0_limits);
	kfree(p2p_limits);
	kfree(mbss_limits);
	kfree(combo);
	return -ENOMEM;
}
