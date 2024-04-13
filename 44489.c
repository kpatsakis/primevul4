void ath_update_max_aggr_framelen(struct ath_softc *sc, int queue, int txop)
{
	u16 *cur_ht20, *cur_ht20_sgi, *cur_ht40, *cur_ht40_sgi;
	int mcs;

	/* 4ms is the default (and maximum) duration */
	if (!txop || txop > 4096)
		txop = 4096;

	cur_ht20 = sc->tx.max_aggr_framelen[queue][MCS_HT20];
	cur_ht20_sgi = sc->tx.max_aggr_framelen[queue][MCS_HT20_SGI];
	cur_ht40 = sc->tx.max_aggr_framelen[queue][MCS_HT40];
	cur_ht40_sgi = sc->tx.max_aggr_framelen[queue][MCS_HT40_SGI];
	for (mcs = 0; mcs < 32; mcs++) {
		cur_ht20[mcs] = ath_max_framelen(txop, mcs, false, false);
		cur_ht20_sgi[mcs] = ath_max_framelen(txop, mcs, false, true);
		cur_ht40[mcs] = ath_max_framelen(txop, mcs, true, false);
		cur_ht40_sgi[mcs] = ath_max_framelen(txop, mcs, true, true);
	}
}
