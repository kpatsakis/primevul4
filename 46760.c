static void sync_sbs(struct mddev *mddev, int nospares)
{
	/* Update each superblock (in-memory image), but
	 * if we are allowed to, skip spares which already
	 * have the right event counter, or have one earlier
	 * (which would mean they aren't being marked as dirty
	 * with the rest of the array)
	 */
	struct md_rdev *rdev;
	rdev_for_each(rdev, mddev) {
		if (rdev->sb_events == mddev->events ||
		    (nospares &&
		     rdev->raid_disk < 0 &&
		     rdev->sb_events+1 == mddev->events)) {
			/* Don't update this superblock */
			rdev->sb_loaded = 2;
		} else {
			sync_super(mddev, rdev);
			rdev->sb_loaded = 1;
		}
	}
}
