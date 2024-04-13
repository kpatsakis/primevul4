xfs_free_buftarg(
	struct xfs_mount	*mp,
	struct xfs_buftarg	*btp)
{
	unregister_shrinker(&btp->bt_shrinker);

	if (mp->m_flags & XFS_MOUNT_BARRIER)
		xfs_blkdev_issue_flush(btp);

	kmem_free(btp);
}
