static int blkdev_close(struct inode * inode, struct file * filp)
{
	struct block_device *bdev = I_BDEV(filp->f_mapping->host);
	blkdev_put(bdev, filp->f_mode);
	return 0;
}
