static bool bf_is_ampdu_not_probing(struct ath_buf *bf)
{
    struct ieee80211_tx_info *info = IEEE80211_SKB_CB(bf->bf_mpdu);
    return bf_isampdu(bf) && !(info->flags & IEEE80211_TX_CTL_RATE_CTRL_PROBE);
}
