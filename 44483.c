u8 ath_txchainmask_reduction(struct ath_softc *sc, u8 chainmask, u32 rate)
{
	struct ath_hw *ah = sc->sc_ah;
	struct ath9k_channel *curchan = ah->curchan;

	if ((ah->caps.hw_caps & ATH9K_HW_CAP_APM) && IS_CHAN_5GHZ(curchan) &&
	    (chainmask == 0x7) && (rate < 0x90))
		return 0x3;
	else if (AR_SREV_9462(ah) && ath9k_hw_btcoex_is_enabled(ah) &&
		 IS_CCK_RATE(rate))
		return 0x2;
	else
		return chainmask;
}
