void bd_set_size(struct block_device *bdev, loff_t size)
{
	unsigned bsize = bdev_logical_block_size(bdev);

	mutex_lock(&bdev->bd_inode->i_mutex);
	i_size_write(bdev->bd_inode, size);
	mutex_unlock(&bdev->bd_inode->i_mutex);
	while (bsize < PAGE_CACHE_SIZE) {
		if (size & bsize)
			break;
		bsize <<= 1;
	}
	bdev->bd_block_size = bsize;
	bdev->bd_inode->i_blkbits = blksize_bits(bsize);
}
