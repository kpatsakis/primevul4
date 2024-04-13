void ath_tx_node_init(struct ath_softc *sc, struct ath_node *an)
{
	struct ath_atx_tid *tid;
	struct ath_atx_ac *ac;
	int tidno, acno;

	for (tidno = 0, tid = &an->tid[tidno];
	     tidno < IEEE80211_NUM_TIDS;
	     tidno++, tid++) {
		tid->an        = an;
		tid->tidno     = tidno;
		tid->seq_start = tid->seq_next = 0;
		tid->baw_size  = WME_MAX_BA;
		tid->baw_head  = tid->baw_tail = 0;
		tid->sched     = false;
		tid->paused    = false;
		tid->active	   = false;
		__skb_queue_head_init(&tid->buf_q);
		__skb_queue_head_init(&tid->retry_q);
		acno = TID_TO_WME_AC(tidno);
		tid->ac = &an->ac[acno];
	}

	for (acno = 0, ac = &an->ac[acno];
	     acno < IEEE80211_NUM_ACS; acno++, ac++) {
		ac->sched    = false;
		ac->clear_ps_filter = true;
		ac->txq = sc->tx.txq_map[acno];
		INIT_LIST_HEAD(&ac->tid_q);
	}
}
