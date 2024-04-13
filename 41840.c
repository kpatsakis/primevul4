xfs_buftarg_shrink(
	struct shrinker		*shrink,
	struct shrink_control	*sc)
{
	struct xfs_buftarg	*btp = container_of(shrink,
					struct xfs_buftarg, bt_shrinker);
	struct xfs_buf		*bp;
	int nr_to_scan = sc->nr_to_scan;
	LIST_HEAD(dispose);

	if (!nr_to_scan)
		return btp->bt_lru_nr;

	spin_lock(&btp->bt_lru_lock);
	while (!list_empty(&btp->bt_lru)) {
		if (nr_to_scan-- <= 0)
			break;

		bp = list_first_entry(&btp->bt_lru, struct xfs_buf, b_lru);

		/*
		 * Decrement the b_lru_ref count unless the value is already
		 * zero. If the value is already zero, we need to reclaim the
		 * buffer, otherwise it gets another trip through the LRU.
		 */
		if (!atomic_add_unless(&bp->b_lru_ref, -1, 0)) {
			list_move_tail(&bp->b_lru, &btp->bt_lru);
			continue;
		}

		/*
		 * remove the buffer from the LRU now to avoid needing another
		 * lock round trip inside xfs_buf_rele().
		 */
		list_move(&bp->b_lru, &dispose);
		btp->bt_lru_nr--;
		bp->b_lru_flags |= _XBF_LRU_DISPOSE;
	}
	spin_unlock(&btp->bt_lru_lock);

	while (!list_empty(&dispose)) {
		bp = list_first_entry(&dispose, struct xfs_buf, b_lru);
		list_del_init(&bp->b_lru);
		xfs_buf_rele(bp);
	}

	return btp->bt_lru_nr;
}
