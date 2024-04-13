int ath_cabq_update(struct ath_softc *sc)
{
	struct ath9k_tx_queue_info qi;
	struct ath_beacon_config *cur_conf = &sc->cur_beacon_conf;
	int qnum = sc->beacon.cabq->axq_qnum;

	ath9k_hw_get_txq_props(sc->sc_ah, qnum, &qi);

	qi.tqi_readyTime = (cur_conf->beacon_interval *
			    ATH_CABQ_READY_TIME) / 100;
	ath_txq_update(sc, qnum, &qi);

	return 0;
}
