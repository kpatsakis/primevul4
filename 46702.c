raid_disks_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned int n;
	int err;

	err = kstrtouint(buf, 10, &n);
	if (err < 0)
		return err;

	err = mddev_lock(mddev);
	if (err)
		return err;
	if (mddev->pers)
		err = update_raid_disks(mddev, n);
	else if (mddev->reshape_position != MaxSector) {
		struct md_rdev *rdev;
		int olddisks = mddev->raid_disks - mddev->delta_disks;

		err = -EINVAL;
		rdev_for_each(rdev, mddev) {
			if (olddisks < n &&
			    rdev->data_offset < rdev->new_data_offset)
				goto out_unlock;
			if (olddisks > n &&
			    rdev->data_offset > rdev->new_data_offset)
				goto out_unlock;
		}
		err = 0;
		mddev->delta_disks = n - olddisks;
		mddev->raid_disks = n;
		mddev->reshape_backwards = (mddev->delta_disks < 0);
	} else
		mddev->raid_disks = n;
out_unlock:
	mddev_unlock(mddev);
	return err ? err : len;
}
