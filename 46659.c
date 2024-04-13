void md_reload_sb(struct mddev *mddev)
{
	struct md_rdev *rdev, *tmp;

	rdev_for_each_safe(rdev, tmp, mddev) {
		rdev->sb_loaded = 0;
		ClearPageUptodate(rdev->sb_page);
	}
	mddev->raid_disks = 0;
	analyze_sbs(mddev);
	rdev_for_each_safe(rdev, tmp, mddev) {
		struct mdp_superblock_1 *sb = page_address(rdev->sb_page);
		/* since we don't write to faulty devices, we figure out if the
		 *  disk is faulty by comparing events
		 */
		if (mddev->events > sb->events)
			set_bit(Faulty, &rdev->flags);
	}

}
