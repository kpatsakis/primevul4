reshape_position_store(struct mddev *mddev, const char *buf, size_t len)
{
	struct md_rdev *rdev;
	unsigned long long new;
	int err;

	err = kstrtoull(buf, 10, &new);
	if (err < 0)
		return err;
	if (new != (sector_t)new)
		return -EINVAL;
	err = mddev_lock(mddev);
	if (err)
		return err;
	err = -EBUSY;
	if (mddev->pers)
		goto unlock;
	mddev->reshape_position = new;
	mddev->delta_disks = 0;
	mddev->reshape_backwards = 0;
	mddev->new_level = mddev->level;
	mddev->new_layout = mddev->layout;
	mddev->new_chunk_sectors = mddev->chunk_sectors;
	rdev_for_each(rdev, mddev)
		rdev->new_data_offset = rdev->data_offset;
	err = 0;
unlock:
	mddev_unlock(mddev);
	return err ?: len;
}
