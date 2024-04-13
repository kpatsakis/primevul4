xfs_buf_lru_del(
	struct xfs_buf	*bp)
{
	struct xfs_buftarg *btp = bp->b_target;

	if (list_empty(&bp->b_lru))
		return;

	spin_lock(&btp->bt_lru_lock);
	if (!list_empty(&bp->b_lru)) {
		list_del_init(&bp->b_lru);
		btp->bt_lru_nr--;
	}
	spin_unlock(&btp->bt_lru_lock);
}
