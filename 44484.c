void ath_txq_schedule(struct ath_softc *sc, struct ath_txq *txq)
{
	struct ath_atx_ac *ac, *last_ac;
	struct ath_atx_tid *tid, *last_tid;
	bool sent = false;

	if (test_bit(SC_OP_HW_RESET, &sc->sc_flags) ||
	    list_empty(&txq->axq_acq))
		return;

	rcu_read_lock();

	last_ac = list_entry(txq->axq_acq.prev, struct ath_atx_ac, list);
	while (!list_empty(&txq->axq_acq)) {
		bool stop = false;

		ac = list_first_entry(&txq->axq_acq, struct ath_atx_ac, list);
		last_tid = list_entry(ac->tid_q.prev, struct ath_atx_tid, list);
		list_del(&ac->list);
		ac->sched = false;

		while (!list_empty(&ac->tid_q)) {

			tid = list_first_entry(&ac->tid_q, struct ath_atx_tid,
					       list);
			list_del(&tid->list);
			tid->sched = false;

			if (tid->paused)
				continue;

			if (ath_tx_sched_aggr(sc, txq, tid, &stop))
				sent = true;

			/*
			 * add tid to round-robin queue if more frames
			 * are pending for the tid
			 */
			if (ath_tid_has_buffered(tid))
				ath_tx_queue_tid(txq, tid);

			if (stop || tid == last_tid)
				break;
		}

		if (!list_empty(&ac->tid_q) && !ac->sched) {
			ac->sched = true;
			list_add_tail(&ac->list, &txq->axq_acq);
		}

		if (stop)
			break;

		if (ac == last_ac) {
			if (!sent)
				break;

			sent = false;
			last_ac = list_entry(txq->axq_acq.prev,
					     struct ath_atx_ac, list);
		}
	}

	rcu_read_unlock();
}
