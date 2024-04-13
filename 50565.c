static int brcmf_construct_chaninfo(struct brcmf_cfg80211_info *cfg,
				    u32 bw_cap[])
{
	struct brcmf_if *ifp = netdev_priv(cfg_to_ndev(cfg));
	struct ieee80211_supported_band *band;
	struct ieee80211_channel *channel;
	struct wiphy *wiphy;
	struct brcmf_chanspec_list *list;
	struct brcmu_chan ch;
	int err;
	u8 *pbuf;
	u32 i, j;
	u32 total;
	u32 chaninfo;
	u32 index;

	pbuf = kzalloc(BRCMF_DCMD_MEDLEN, GFP_KERNEL);

	if (pbuf == NULL)
		return -ENOMEM;

	list = (struct brcmf_chanspec_list *)pbuf;

	err = brcmf_fil_iovar_data_get(ifp, "chanspecs", pbuf,
				       BRCMF_DCMD_MEDLEN);
	if (err) {
		brcmf_err("get chanspecs error (%d)\n", err);
		goto fail_pbuf;
	}

	wiphy = cfg_to_wiphy(cfg);
	band = wiphy->bands[NL80211_BAND_2GHZ];
	if (band)
		for (i = 0; i < band->n_channels; i++)
			band->channels[i].flags = IEEE80211_CHAN_DISABLED;
	band = wiphy->bands[NL80211_BAND_5GHZ];
	if (band)
		for (i = 0; i < band->n_channels; i++)
			band->channels[i].flags = IEEE80211_CHAN_DISABLED;

	total = le32_to_cpu(list->count);
	for (i = 0; i < total; i++) {
		ch.chspec = (u16)le32_to_cpu(list->element[i]);
		cfg->d11inf.decchspec(&ch);

		if (ch.band == BRCMU_CHAN_BAND_2G) {
			band = wiphy->bands[NL80211_BAND_2GHZ];
		} else if (ch.band == BRCMU_CHAN_BAND_5G) {
			band = wiphy->bands[NL80211_BAND_5GHZ];
		} else {
			brcmf_err("Invalid channel Spec. 0x%x.\n", ch.chspec);
			continue;
		}
		if (!band)
			continue;
		if (!(bw_cap[band->band] & WLC_BW_40MHZ_BIT) &&
		    ch.bw == BRCMU_CHAN_BW_40)
			continue;
		if (!(bw_cap[band->band] & WLC_BW_80MHZ_BIT) &&
		    ch.bw == BRCMU_CHAN_BW_80)
			continue;

		channel = band->channels;
		index = band->n_channels;
		for (j = 0; j < band->n_channels; j++) {
			if (channel[j].hw_value == ch.control_ch_num) {
				index = j;
				break;
			}
		}
		channel[index].center_freq =
			ieee80211_channel_to_frequency(ch.control_ch_num,
						       band->band);
		channel[index].hw_value = ch.control_ch_num;

		/* assuming the chanspecs order is HT20,
		 * HT40 upper, HT40 lower, and VHT80.
		 */
		if (ch.bw == BRCMU_CHAN_BW_80) {
			channel[index].flags &= ~IEEE80211_CHAN_NO_80MHZ;
		} else if (ch.bw == BRCMU_CHAN_BW_40) {
			brcmf_update_bw40_channel_flag(&channel[index], &ch);
		} else {
			/* enable the channel and disable other bandwidths
			 * for now as mentioned order assure they are enabled
			 * for subsequent chanspecs.
			 */
			channel[index].flags = IEEE80211_CHAN_NO_HT40 |
					       IEEE80211_CHAN_NO_80MHZ;
			ch.bw = BRCMU_CHAN_BW_20;
			cfg->d11inf.encchspec(&ch);
			chaninfo = ch.chspec;
			err = brcmf_fil_bsscfg_int_get(ifp, "per_chan_info",
						       &chaninfo);
			if (!err) {
				if (chaninfo & WL_CHAN_RADAR)
					channel[index].flags |=
						(IEEE80211_CHAN_RADAR |
						 IEEE80211_CHAN_NO_IR);
				if (chaninfo & WL_CHAN_PASSIVE)
					channel[index].flags |=
						IEEE80211_CHAN_NO_IR;
			}
		}
	}

fail_pbuf:
	kfree(pbuf);
	return err;
}
