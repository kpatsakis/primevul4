suspend_hi_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned long long old, new;
	int err;

	err = kstrtoull(buf, 10, &new);
	if (err < 0)
		return err;
	if (new != (sector_t)new)
		return -EINVAL;

	err = mddev_lock(mddev);
	if (err)
		return err;
	err = -EINVAL;
	if (mddev->pers == NULL ||
	    mddev->pers->quiesce == NULL)
		goto unlock;
	old = mddev->suspend_hi;
	mddev->suspend_hi = new;
	if (new <= old)
		/* Shrinking suspended region */
		mddev->pers->quiesce(mddev, 2);
	else {
		/* Expanding suspended region - need to wait */
		mddev->pers->quiesce(mddev, 1);
		mddev->pers->quiesce(mddev, 0);
	}
	err = 0;
unlock:
	mddev_unlock(mddev);
	return err ?: len;
}
