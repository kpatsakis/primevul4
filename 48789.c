void bdput(struct block_device *bdev)
{
	iput(bdev->bd_inode);
}
