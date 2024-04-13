int rdev_set_badblocks(struct md_rdev *rdev, sector_t s, int sectors,
		       int is_new)
{
	int rv;
	if (is_new)
		s += rdev->new_data_offset;
	else
		s += rdev->data_offset;
	rv = md_set_badblocks(&rdev->badblocks,
			      s, sectors, 0);
	if (rv) {
		/* Make sure they get written out promptly */
		sysfs_notify_dirent_safe(rdev->sysfs_state);
		set_bit(MD_CHANGE_CLEAN, &rdev->mddev->flags);
		md_wakeup_thread(rdev->mddev->thread);
	}
	return rv;
}
