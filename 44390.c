static void sta_unblock(struct work_struct *wk)
{
	struct sta_info *sta;

	sta = container_of(wk, struct sta_info, drv_unblock_wk);

	if (sta->dead)
		return;

	if (!test_sta_flag(sta, WLAN_STA_PS_STA)) {
		local_bh_disable();
		ieee80211_sta_ps_deliver_wakeup(sta);
		local_bh_enable();
	} else if (test_and_clear_sta_flag(sta, WLAN_STA_PSPOLL)) {
		clear_sta_flag(sta, WLAN_STA_PS_DRIVER);

		local_bh_disable();
		ieee80211_sta_ps_deliver_poll_response(sta);
		local_bh_enable();
	} else if (test_and_clear_sta_flag(sta, WLAN_STA_UAPSD)) {
		clear_sta_flag(sta, WLAN_STA_PS_DRIVER);

		local_bh_disable();
		ieee80211_sta_ps_deliver_uapsd(sta);
		local_bh_enable();
	} else
		clear_sta_flag(sta, WLAN_STA_PS_DRIVER);
}
