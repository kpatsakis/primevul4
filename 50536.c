brcmf_cfg80211_get_station_ibss(struct brcmf_if *ifp,
				struct station_info *sinfo)
{
	struct brcmf_scb_val_le scbval;
	struct brcmf_pktcnt_le pktcnt;
	s32 err;
	u32 rate;
	u32 rssi;

	/* Get the current tx rate */
	err = brcmf_fil_cmd_int_get(ifp, BRCMF_C_GET_RATE, &rate);
	if (err < 0) {
		brcmf_err("BRCMF_C_GET_RATE error (%d)\n", err);
		return err;
	}
	sinfo->filled |= BIT(NL80211_STA_INFO_TX_BITRATE);
	sinfo->txrate.legacy = rate * 5;

	memset(&scbval, 0, sizeof(scbval));
	err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_RSSI, &scbval,
				     sizeof(scbval));
	if (err) {
		brcmf_err("BRCMF_C_GET_RSSI error (%d)\n", err);
		return err;
	}
	rssi = le32_to_cpu(scbval.val);
	sinfo->filled |= BIT(NL80211_STA_INFO_SIGNAL);
	sinfo->signal = rssi;

	err = brcmf_fil_cmd_data_get(ifp, BRCMF_C_GET_GET_PKTCNTS, &pktcnt,
				     sizeof(pktcnt));
	if (err) {
		brcmf_err("BRCMF_C_GET_GET_PKTCNTS error (%d)\n", err);
		return err;
	}
	sinfo->filled |= BIT(NL80211_STA_INFO_RX_PACKETS) |
			 BIT(NL80211_STA_INFO_RX_DROP_MISC) |
			 BIT(NL80211_STA_INFO_TX_PACKETS) |
			 BIT(NL80211_STA_INFO_TX_FAILED);
	sinfo->rx_packets = le32_to_cpu(pktcnt.rx_good_pkt);
	sinfo->rx_dropped_misc = le32_to_cpu(pktcnt.rx_bad_pkt);
	sinfo->tx_packets = le32_to_cpu(pktcnt.tx_good_pkt);
	sinfo->tx_failed  = le32_to_cpu(pktcnt.tx_bad_pkt);

	return 0;
}
