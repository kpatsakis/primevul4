brcmf_cfg80211_get_station(struct wiphy *wiphy, struct net_device *ndev,
			   const u8 *mac, struct station_info *sinfo)
{
	struct brcmf_if *ifp = netdev_priv(ndev);
	struct brcmf_scb_val_le scb_val;
	s32 err = 0;
	struct brcmf_sta_info_le sta_info_le;
	u32 sta_flags;
	u32 is_tdls_peer;
	s32 total_rssi;
	s32 count_rssi;
	int rssi;
	u32 i;

	brcmf_dbg(TRACE, "Enter, MAC %pM\n", mac);
	if (!check_vif_up(ifp->vif))
		return -EIO;

	if (brcmf_is_ibssmode(ifp->vif))
		return brcmf_cfg80211_get_station_ibss(ifp, sinfo);

	memset(&sta_info_le, 0, sizeof(sta_info_le));
	memcpy(&sta_info_le, mac, ETH_ALEN);
	err = brcmf_fil_iovar_data_get(ifp, "tdls_sta_info",
				       &sta_info_le,
				       sizeof(sta_info_le));
	is_tdls_peer = !err;
	if (err) {
		err = brcmf_fil_iovar_data_get(ifp, "sta_info",
					       &sta_info_le,
					       sizeof(sta_info_le));
		if (err < 0) {
			brcmf_err("GET STA INFO failed, %d\n", err);
			goto done;
		}
	}
	brcmf_dbg(TRACE, "version %d\n", le16_to_cpu(sta_info_le.ver));
	sinfo->filled = BIT(NL80211_STA_INFO_INACTIVE_TIME);
	sinfo->inactive_time = le32_to_cpu(sta_info_le.idle) * 1000;
	sta_flags = le32_to_cpu(sta_info_le.flags);
	brcmf_convert_sta_flags(sta_flags, sinfo);
	sinfo->sta_flags.mask |= BIT(NL80211_STA_FLAG_TDLS_PEER);
	if (is_tdls_peer)
		sinfo->sta_flags.set |= BIT(NL80211_STA_FLAG_TDLS_PEER);
	else
		sinfo->sta_flags.set &= ~BIT(NL80211_STA_FLAG_TDLS_PEER);
	if (sta_flags & BRCMF_STA_ASSOC) {
		sinfo->filled |= BIT(NL80211_STA_INFO_CONNECTED_TIME);
		sinfo->connected_time = le32_to_cpu(sta_info_le.in);
		brcmf_fill_bss_param(ifp, sinfo);
	}
	if (sta_flags & BRCMF_STA_SCBSTATS) {
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_FAILED);
		sinfo->tx_failed = le32_to_cpu(sta_info_le.tx_failures);
		sinfo->filled |= BIT(NL80211_STA_INFO_TX_PACKETS);
		sinfo->tx_packets = le32_to_cpu(sta_info_le.tx_pkts);
		sinfo->tx_packets += le32_to_cpu(sta_info_le.tx_mcast_pkts);
		sinfo->filled |= BIT(NL80211_STA_INFO_RX_PACKETS);
		sinfo->rx_packets = le32_to_cpu(sta_info_le.rx_ucast_pkts);
		sinfo->rx_packets += le32_to_cpu(sta_info_le.rx_mcast_pkts);
		if (sinfo->tx_packets) {
			sinfo->filled |= BIT(NL80211_STA_INFO_TX_BITRATE);
			sinfo->txrate.legacy =
				le32_to_cpu(sta_info_le.tx_rate) / 100;
		}
		if (sinfo->rx_packets) {
			sinfo->filled |= BIT(NL80211_STA_INFO_RX_BITRATE);
			sinfo->rxrate.legacy =
				le32_to_cpu(sta_info_le.rx_rate) / 100;
		}
		if (le16_to_cpu(sta_info_le.ver) >= 4) {
			sinfo->filled |= BIT(NL80211_STA_INFO_TX_BYTES);
			sinfo->tx_bytes = le64_to_cpu(sta_info_le.tx_tot_bytes);
			sinfo->filled |= BIT(NL80211_STA_INFO_RX_BYTES);
			sinfo->rx_bytes = le64_to_cpu(sta_info_le.rx_tot_bytes);
		}
		total_rssi = 0;
		count_rssi = 0;
		for (i = 0; i < BRCMF_ANT_MAX; i++) {
			if (sta_info_le.rssi[i]) {
				sinfo->chain_signal_avg[count_rssi] =
					sta_info_le.rssi[i];
				sinfo->chain_signal[count_rssi] =
					sta_info_le.rssi[i];
				total_rssi += sta_info_le.rssi[i];
				count_rssi++;
			}
		}
		if (count_rssi) {
			sinfo->filled |= BIT(NL80211_STA_INFO_CHAIN_SIGNAL);
			sinfo->chains = count_rssi;

			sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
			total_rssi /= count_rssi;
			sinfo->signal = total_rssi;
		} else if (test_bit(BRCMF_VIF_STATUS_CONNECTED,
			&ifp->vif->sme_state)) {
			memset(&scb_val, 0, sizeof(scb_val));
			err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_RSSI,
						     &scb_val, sizeof(scb_val));
			if (err) {
				brcmf_err("Could not get rssi (%d)\n", err);
				goto done;
			} else {
				rssi = le32_to_cpu(scb_val.val);
				sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
				sinfo->signal = rssi;
				brcmf_dbg(CONN, "RSSI %d dBm\n", rssi);
			}
		}
	}
done:
	brcmf_dbg(TRACE, "Exit\n");
	return err;
}
