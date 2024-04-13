print_chaninfo(netdissect_options *ndo,
               uint16_t freq, int flags, int presentflags)
{
	ND_PRINT((ndo, "%u MHz", freq));
	if (presentflags & (1 << IEEE80211_RADIOTAP_MCS)) {
		/*
		 * We have the MCS field, so this is 11n, regardless
		 * of what the channel flags say.
		 */
		ND_PRINT((ndo, " 11n"));
	} else {
		if (IS_CHAN_FHSS(flags))
			ND_PRINT((ndo, " FHSS"));
		if (IS_CHAN_A(flags)) {
			if (flags & IEEE80211_CHAN_HALF)
				ND_PRINT((ndo, " 11a/10Mhz"));
			else if (flags & IEEE80211_CHAN_QUARTER)
				ND_PRINT((ndo, " 11a/5Mhz"));
			else
				ND_PRINT((ndo, " 11a"));
		}
		if (IS_CHAN_ANYG(flags)) {
			if (flags & IEEE80211_CHAN_HALF)
				ND_PRINT((ndo, " 11g/10Mhz"));
			else if (flags & IEEE80211_CHAN_QUARTER)
				ND_PRINT((ndo, " 11g/5Mhz"));
			else
				ND_PRINT((ndo, " 11g"));
		} else if (IS_CHAN_B(flags))
			ND_PRINT((ndo, " 11b"));
		if (flags & IEEE80211_CHAN_TURBO)
			ND_PRINT((ndo, " Turbo"));
	}
	/*
	 * These apply to 11n.
	 */
	if (flags & IEEE80211_CHAN_HT20)
		ND_PRINT((ndo, " ht/20"));
	else if (flags & IEEE80211_CHAN_HT40D)
		ND_PRINT((ndo, " ht/40-"));
	else if (flags & IEEE80211_CHAN_HT40U)
		ND_PRINT((ndo, " ht/40+"));
	ND_PRINT((ndo, " "));
}
