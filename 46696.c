min_sync_store(struct mddev *mddev, const char *buf, size_t len)
{
	unsigned long long min;
	int err;

	if (kstrtoull(buf, 10, &min))
		return -EINVAL;

	spin_lock(&mddev->lock);
	err = -EINVAL;
	if (min > mddev->resync_max)
		goto out_unlock;

	err = -EBUSY;
	if (test_bit(MD_RECOVERY_RUNNING, &mddev->recovery))
		goto out_unlock;

	/* Round down to multiple of 4K for safety */
	mddev->resync_min = round_down(min, 8);
	err = 0;

out_unlock:
	spin_unlock(&mddev->lock);
	return err ?: len;
}
