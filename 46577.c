static int alloc_disk_sb(struct md_rdev *rdev)
{
	rdev->sb_page = alloc_page(GFP_KERNEL);
	if (!rdev->sb_page) {
		printk(KERN_ALERT "md: out of memory.\n");
		return -ENOMEM;
	}

	return 0;
}
