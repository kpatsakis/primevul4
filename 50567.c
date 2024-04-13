static void brcmf_convert_sta_flags(u32 fw_sta_flags, struct station_info *si)
{
	struct nl80211_sta_flag_update *sfu;

	brcmf_dbg(TRACE, "flags %08x\n", fw_sta_flags);
	si->filled |= BIT(NL80211_STA_INFO_STA_FLAGS);
	sfu = &si->sta_flags;
	sfu->mask = BIT(NL80211_STA_FLAG_WME) |
		    BIT(NL80211_STA_FLAG_AUTHENTICATED) |
		    BIT(NL80211_STA_FLAG_ASSOCIATED) |
		    BIT(NL80211_STA_FLAG_AUTHORIZED);
	if (fw_sta_flags & BRCMF_STA_WME)
		sfu->set |= BIT(NL80211_STA_FLAG_WME);
	if (fw_sta_flags & BRCMF_STA_AUTHE)
		sfu->set |= BIT(NL80211_STA_FLAG_AUTHENTICATED);
	if (fw_sta_flags & BRCMF_STA_ASSOC)
		sfu->set |= BIT(NL80211_STA_FLAG_ASSOCIATED);
	if (fw_sta_flags & BRCMF_STA_AUTHO)
		sfu->set |= BIT(NL80211_STA_FLAG_AUTHORIZED);
}
