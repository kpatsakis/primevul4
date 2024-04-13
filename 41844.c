xfs_setsize_buftarg_early(
	xfs_buftarg_t		*btp,
	struct block_device	*bdev)
{
	return xfs_setsize_buftarg_flags(btp,
			PAGE_SIZE, bdev_logical_block_size(bdev), 0);
}
