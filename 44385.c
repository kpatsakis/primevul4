int sta_info_move_state(struct sta_info *sta,
			enum ieee80211_sta_state new_state)
{
	might_sleep();

	if (sta->sta_state == new_state)
		return 0;

	/* check allowed transitions first */

	switch (new_state) {
	case IEEE80211_STA_NONE:
		if (sta->sta_state != IEEE80211_STA_AUTH)
			return -EINVAL;
		break;
	case IEEE80211_STA_AUTH:
		if (sta->sta_state != IEEE80211_STA_NONE &&
		    sta->sta_state != IEEE80211_STA_ASSOC)
			return -EINVAL;
		break;
	case IEEE80211_STA_ASSOC:
		if (sta->sta_state != IEEE80211_STA_AUTH &&
		    sta->sta_state != IEEE80211_STA_AUTHORIZED)
			return -EINVAL;
		break;
	case IEEE80211_STA_AUTHORIZED:
		if (sta->sta_state != IEEE80211_STA_ASSOC)
			return -EINVAL;
		break;
	default:
		WARN(1, "invalid state %d", new_state);
		return -EINVAL;
	}

	sta_dbg(sta->sdata, "moving STA %pM to state %d\n",
		sta->sta.addr, new_state);

	/*
	 * notify the driver before the actual changes so it can
	 * fail the transition
	 */
	if (test_sta_flag(sta, WLAN_STA_INSERTED)) {
		int err = drv_sta_state(sta->local, sta->sdata, sta,
					sta->sta_state, new_state);
		if (err)
			return err;
	}

	/* reflect the change in all state variables */

	switch (new_state) {
	case IEEE80211_STA_NONE:
		if (sta->sta_state == IEEE80211_STA_AUTH)
			clear_bit(WLAN_STA_AUTH, &sta->_flags);
		break;
	case IEEE80211_STA_AUTH:
		if (sta->sta_state == IEEE80211_STA_NONE)
			set_bit(WLAN_STA_AUTH, &sta->_flags);
		else if (sta->sta_state == IEEE80211_STA_ASSOC)
			clear_bit(WLAN_STA_ASSOC, &sta->_flags);
		break;
	case IEEE80211_STA_ASSOC:
		if (sta->sta_state == IEEE80211_STA_AUTH) {
			set_bit(WLAN_STA_ASSOC, &sta->_flags);
		} else if (sta->sta_state == IEEE80211_STA_AUTHORIZED) {
			if (sta->sdata->vif.type == NL80211_IFTYPE_AP ||
			    (sta->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
			     !sta->sdata->u.vlan.sta))
				atomic_dec(&sta->sdata->bss->num_mcast_sta);
			clear_bit(WLAN_STA_AUTHORIZED, &sta->_flags);
		}
		break;
	case IEEE80211_STA_AUTHORIZED:
		if (sta->sta_state == IEEE80211_STA_ASSOC) {
			if (sta->sdata->vif.type == NL80211_IFTYPE_AP ||
			    (sta->sdata->vif.type == NL80211_IFTYPE_AP_VLAN &&
			     !sta->sdata->u.vlan.sta))
				atomic_inc(&sta->sdata->bss->num_mcast_sta);
			set_bit(WLAN_STA_AUTHORIZED, &sta->_flags);
		}
		break;
	default:
		break;
	}

	sta->sta_state = new_state;

	return 0;
}
