xfs_alloc_buftarg(
	struct xfs_mount	*mp,
	struct block_device	*bdev,
	int			external,
	const char		*fsname)
{
	xfs_buftarg_t		*btp;

	btp = kmem_zalloc(sizeof(*btp), KM_SLEEP);

	btp->bt_mount = mp;
	btp->bt_dev =  bdev->bd_dev;
	btp->bt_bdev = bdev;
	btp->bt_bdi = blk_get_backing_dev_info(bdev);
	if (!btp->bt_bdi)
		goto error;

	INIT_LIST_HEAD(&btp->bt_lru);
	spin_lock_init(&btp->bt_lru_lock);
	if (xfs_setsize_buftarg_early(btp, bdev))
		goto error;
	btp->bt_shrinker.shrink = xfs_buftarg_shrink;
	btp->bt_shrinker.seeks = DEFAULT_SEEKS;
	register_shrinker(&btp->bt_shrinker);
	return btp;

error:
	kmem_free(btp);
	return NULL;
}
