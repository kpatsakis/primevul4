ath_get_skb_tid(struct ath_softc *sc, struct ath_node *an, struct sk_buff *skb)
{
	u8 tidno = skb->priority & IEEE80211_QOS_CTL_TID_MASK;
	return ATH_AN_2_TID(an, tidno);
}
