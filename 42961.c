ieee80211_tx_h_encrypt(struct ieee80211_tx_data *tx)
{
	if (!tx->key)
		return TX_CONTINUE;

	switch (tx->key->conf.cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		return ieee80211_crypto_wep_encrypt(tx);
	case WLAN_CIPHER_SUITE_TKIP:
		return ieee80211_crypto_tkip_encrypt(tx);
	case WLAN_CIPHER_SUITE_CCMP:
		return ieee80211_crypto_ccmp_encrypt(tx);
	case WLAN_CIPHER_SUITE_AES_CMAC:
		return ieee80211_crypto_aes_cmac_encrypt(tx);
	default:
		return ieee80211_crypto_hw_encrypt(tx);
	}

	return TX_DROP;
}
