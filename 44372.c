static unsigned long ieee80211_tids_for_ac(int ac)
{
	/* If we ever support TIDs > 7, this obviously needs to be adjusted */
	switch (ac) {
	case IEEE80211_AC_VO:
		return BIT(6) | BIT(7);
	case IEEE80211_AC_VI:
		return BIT(4) | BIT(5);
	case IEEE80211_AC_BE:
		return BIT(0) | BIT(3);
	case IEEE80211_AC_BK:
		return BIT(1) | BIT(2);
	default:
		WARN_ON(1);
		return 0;
	}
}
