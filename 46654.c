void md_rdev_clear(struct md_rdev *rdev)
{
	if (rdev->sb_page) {
		put_page(rdev->sb_page);
		rdev->sb_loaded = 0;
		rdev->sb_page = NULL;
		rdev->sb_start = 0;
		rdev->sectors = 0;
	}
	if (rdev->bb_page) {
		put_page(rdev->bb_page);
		rdev->bb_page = NULL;
	}
	kfree(rdev->badblocks.page);
	rdev->badblocks.page = NULL;
}
