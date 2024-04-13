static void ath_tx_addto_baw(struct ath_softc *sc, struct ath_atx_tid *tid,
			     struct ath_buf *bf)
{
	struct ath_frame_info *fi = get_frame_info(bf->bf_mpdu);
	u16 seqno = bf->bf_state.seqno;
	int index, cindex;

	index  = ATH_BA_INDEX(tid->seq_start, seqno);
	cindex = (tid->baw_head + index) & (ATH_TID_MAX_BUFS - 1);
	__set_bit(cindex, tid->tx_buf);
	fi->baw_tracked = 1;

	if (index >= ((tid->baw_tail - tid->baw_head) &
		(ATH_TID_MAX_BUFS - 1))) {
		tid->baw_tail = cindex;
		INCR(tid->baw_tail, ATH_TID_MAX_BUFS);
	}
}
