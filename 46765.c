static void unlock_rdev(struct md_rdev *rdev)
{
	struct block_device *bdev = rdev->bdev;
	rdev->bdev = NULL;
	blkdev_put(bdev, FMODE_READ|FMODE_WRITE|FMODE_EXCL);
}
