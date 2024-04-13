int nfs_writepages(struct address_space *mapping, struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;
	unsigned long *bitlock = &NFS_I(inode)->flags;
	struct nfs_pageio_descriptor pgio;
	int err;

	/* Stop dirtying of new pages while we sync */
	err = wait_on_bit_lock(bitlock, NFS_INO_FLUSHING,
			nfs_wait_bit_killable, TASK_KILLABLE);
	if (err)
		goto out_err;

	nfs_inc_stats(inode, NFSIOS_VFSWRITEPAGES);

	NFS_PROTO(inode)->write_pageio_init(&pgio, inode, wb_priority(wbc), &nfs_async_write_completion_ops);
	err = write_cache_pages(mapping, wbc, nfs_writepages_callback, &pgio);
	nfs_pageio_complete(&pgio);

	clear_bit_unlock(NFS_INO_FLUSHING, bitlock);
	smp_mb__after_clear_bit();
	wake_up_bit(bitlock, NFS_INO_FLUSHING);

	if (err < 0)
		goto out_err;
	err = pgio.pg_error;
	if (err < 0)
		goto out_err;
	return 0;
out_err:
	return err;
}
