static void tg3_rss_check_indir_tbl(struct tg3 *tp)
{
	int i;

	if (!tg3_flag(tp, SUPPORT_MSIX))
		return;

	if (tp->rxq_cnt == 1) {
		memset(&tp->rss_ind_tbl[0], 0, sizeof(tp->rss_ind_tbl));
		return;
	}

	/* Validate table against current IRQ count */
	for (i = 0; i < TG3_RSS_INDIR_TBL_SIZE; i++) {
		if (tp->rss_ind_tbl[i] >= tp->rxq_cnt)
			break;
	}

	if (i != TG3_RSS_INDIR_TBL_SIZE)
		tg3_rss_init_dflt_indir_tbl(tp, tp->rxq_cnt);
}
