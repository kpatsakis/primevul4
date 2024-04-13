resync_start_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned long long n;
	int err;

	if (cmd_match(buf, "none"))
		n = MaxSector;
	else {
		err = kstrtoull(buf, 10, &n);
		if (err < 0)
			return err;
		if (n != (sector_t)n)
			return -EINVAL;
	}

	err = mddev_lock(mddev);
	if (err)
		return err;
	if (mddev->pers && !test_bit(MD_RECOVERY_FROZEN, &mddev->recovery))
		err = -EBUSY;

	if (!err) {
		mddev->recovery_cp = n;
		if (mddev->pers)
			set_bit(MD_CHANGE_CLEAN, &mddev->flags);
	}
	mddev_unlock(mddev);
	return err ?: len;
}
