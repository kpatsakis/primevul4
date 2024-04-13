layout_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned int n;
	int err;

	err = kstrtouint(buf, 10, &n);
	if (err < 0)
		return err;
	err = mddev_lock(mddev);
	if (err)
		return err;

	if (mddev->pers) {
		if (mddev->pers->check_reshape == NULL)
			err = -EBUSY;
		else if (mddev->ro)
			err = -EROFS;
		else {
			mddev->new_layout = n;
			err = mddev->pers->check_reshape(mddev);
			if (err)
				mddev->new_layout = mddev->layout;
		}
	} else {
		mddev->new_layout = n;
		if (mddev->reshape_position == MaxSector)
			mddev->layout = n;
	}
	mddev_unlock(mddev);
	return err ?: len;
}
