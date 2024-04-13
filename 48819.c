int sync_blockdev(struct block_device *bdev)
{
	return __sync_blockdev(bdev, 1);
}
