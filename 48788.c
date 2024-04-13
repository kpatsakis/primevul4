struct block_device *bdgrab(struct block_device *bdev)
{
	ihold(bdev->bd_inode);
	return bdev;
}
