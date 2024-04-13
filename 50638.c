static u16 chandef_to_chanspec(struct brcmu_d11inf *d11inf,
			       struct cfg80211_chan_def *ch)
{
	struct brcmu_chan ch_inf;
	s32 primary_offset;

	brcmf_dbg(TRACE, "chandef: control %d center %d width %d\n",
		  ch->chan->center_freq, ch->center_freq1, ch->width);
	ch_inf.chnum = ieee80211_frequency_to_channel(ch->center_freq1);
	primary_offset = ch->chan->center_freq - ch->center_freq1;
	switch (ch->width) {
	case NL80211_CHAN_WIDTH_20:
	case NL80211_CHAN_WIDTH_20_NOHT:
		ch_inf.bw = BRCMU_CHAN_BW_20;
		WARN_ON(primary_offset != 0);
		break;
	case NL80211_CHAN_WIDTH_40:
		ch_inf.bw = BRCMU_CHAN_BW_40;
		if (primary_offset > 0)
			ch_inf.sb = BRCMU_CHAN_SB_U;
		else
			ch_inf.sb = BRCMU_CHAN_SB_L;
		break;
	case NL80211_CHAN_WIDTH_80:
		ch_inf.bw = BRCMU_CHAN_BW_80;
		if (primary_offset == -30)
			ch_inf.sb = BRCMU_CHAN_SB_LL;
		else if (primary_offset == -10)
			ch_inf.sb = BRCMU_CHAN_SB_LU;
		else if (primary_offset == 10)
			ch_inf.sb = BRCMU_CHAN_SB_UL;
		else
			ch_inf.sb = BRCMU_CHAN_SB_UU;
		break;
	case NL80211_CHAN_WIDTH_80P80:
	case NL80211_CHAN_WIDTH_160:
	case NL80211_CHAN_WIDTH_5:
	case NL80211_CHAN_WIDTH_10:
	default:
		WARN_ON_ONCE(1);
	}
	switch (ch->chan->band) {
	case NL80211_BAND_2GHZ:
		ch_inf.band = BRCMU_CHAN_BAND_2G;
		break;
	case NL80211_BAND_5GHZ:
		ch_inf.band = BRCMU_CHAN_BAND_5G;
		break;
	case NL80211_BAND_60GHZ:
	default:
		WARN_ON_ONCE(1);
	}
	d11inf->encchspec(&ch_inf);

	return ch_inf.chspec;
}
