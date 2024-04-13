static void ath_tx_count_frames(struct ath_softc *sc, struct ath_buf *bf,
			        struct ath_tx_status *ts, int txok,
			        int *nframes, int *nbad)
{
	struct ath_frame_info *fi;
	u16 seq_st = 0;
	u32 ba[WME_BA_BMP_SIZE >> 5];
	int ba_index;
	int isaggr = 0;

	*nbad = 0;
	*nframes = 0;

	isaggr = bf_isaggr(bf);
	if (isaggr) {
		seq_st = ts->ts_seqnum;
		memcpy(ba, &ts->ba_low, WME_BA_BMP_SIZE >> 3);
	}

	while (bf) {
		fi = get_frame_info(bf->bf_mpdu);
		ba_index = ATH_BA_INDEX(seq_st, bf->bf_state.seqno);

		(*nframes)++;
		if (!txok || (isaggr && !ATH_BA_ISSET(ba, ba_index)))
			(*nbad)++;

		bf = bf->bf_next;
	}
}
