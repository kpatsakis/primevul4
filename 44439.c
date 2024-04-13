static int ath_compute_num_delims(struct ath_softc *sc, struct ath_atx_tid *tid,
				  struct ath_buf *bf, u16 frmlen,
				  bool first_subfrm)
{
#define FIRST_DESC_NDELIMS 60
	u32 nsymbits, nsymbols;
	u16 minlen;
	u8 flags, rix;
	int width, streams, half_gi, ndelim, mindelim;
	struct ath_frame_info *fi = get_frame_info(bf->bf_mpdu);

	/* Select standard number of delimiters based on frame length alone */
	ndelim = ATH_AGGR_GET_NDELIM(frmlen);

	/*
	 * If encryption enabled, hardware requires some more padding between
	 * subframes.
	 * TODO - this could be improved to be dependent on the rate.
	 *      The hardware can keep up at lower rates, but not higher rates
	 */
	if ((fi->keyix != ATH9K_TXKEYIX_INVALID) &&
	    !(sc->sc_ah->caps.hw_caps & ATH9K_HW_CAP_EDMA))
		ndelim += ATH_AGGR_ENCRYPTDELIM;

	/*
	 * Add delimiter when using RTS/CTS with aggregation
	 * and non enterprise AR9003 card
	 */
	if (first_subfrm && !AR_SREV_9580_10_OR_LATER(sc->sc_ah) &&
	    (sc->sc_ah->ent_mode & AR_ENT_OTP_MIN_PKT_SIZE_DISABLE))
		ndelim = max(ndelim, FIRST_DESC_NDELIMS);

	/*
	 * Convert desired mpdu density from microeconds to bytes based
	 * on highest rate in rate series (i.e. first rate) to determine
	 * required minimum length for subframe. Take into account
	 * whether high rate is 20 or 40Mhz and half or full GI.
	 *
	 * If there is no mpdu density restriction, no further calculation
	 * is needed.
	 */

	if (tid->an->mpdudensity == 0)
		return ndelim;

	rix = bf->rates[0].idx;
	flags = bf->rates[0].flags;
	width = (flags & IEEE80211_TX_RC_40_MHZ_WIDTH) ? 1 : 0;
	half_gi = (flags & IEEE80211_TX_RC_SHORT_GI) ? 1 : 0;

	if (half_gi)
		nsymbols = NUM_SYMBOLS_PER_USEC_HALFGI(tid->an->mpdudensity);
	else
		nsymbols = NUM_SYMBOLS_PER_USEC(tid->an->mpdudensity);

	if (nsymbols == 0)
		nsymbols = 1;

	streams = HT_RC_2_STREAMS(rix);
	nsymbits = bits_per_symbol[rix % 8][width] * streams;
	minlen = (nsymbols * nsymbits) / BITS_PER_BYTE;

	if (frmlen < minlen) {
		mindelim = (minlen - frmlen) / ATH_AGGR_DELIM_SZ;
		ndelim = max(mindelim, ndelim);
	}

	return ndelim;
}
