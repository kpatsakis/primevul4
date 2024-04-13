static int remove_and_add_spares(struct mddev *mddev,
				 struct md_rdev *this)
{
	struct md_rdev *rdev;
	int spares = 0;
	int removed = 0;

	rdev_for_each(rdev, mddev)
		if ((this == NULL || rdev == this) &&
		    rdev->raid_disk >= 0 &&
		    !test_bit(Blocked, &rdev->flags) &&
		    (test_bit(Faulty, &rdev->flags) ||
		     ! test_bit(In_sync, &rdev->flags)) &&
		    atomic_read(&rdev->nr_pending)==0) {
			if (mddev->pers->hot_remove_disk(
				    mddev, rdev) == 0) {
				sysfs_unlink_rdev(mddev, rdev);
				rdev->raid_disk = -1;
				removed++;
			}
		}
	if (removed && mddev->kobj.sd)
		sysfs_notify(&mddev->kobj, NULL, "degraded");

	if (this)
		goto no_add;

	rdev_for_each(rdev, mddev) {
		if (rdev->raid_disk >= 0 &&
		    !test_bit(In_sync, &rdev->flags) &&
		    !test_bit(Faulty, &rdev->flags))
			spares++;
		if (rdev->raid_disk >= 0)
			continue;
		if (test_bit(Faulty, &rdev->flags))
			continue;
		if (mddev->ro &&
		    ! (rdev->saved_raid_disk >= 0 &&
		       !test_bit(Bitmap_sync, &rdev->flags)))
			continue;

		if (rdev->saved_raid_disk < 0)
			rdev->recovery_offset = 0;
		if (mddev->pers->
		    hot_add_disk(mddev, rdev) == 0) {
			if (sysfs_link_rdev(mddev, rdev))
				/* failure here is OK */;
			spares++;
			md_new_event(mddev);
			set_bit(MD_CHANGE_DEVS, &mddev->flags);
		}
	}
no_add:
	if (removed)
		set_bit(MD_CHANGE_DEVS, &mddev->flags);
	return spares;
}
