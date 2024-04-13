int thaw_bdev(struct block_device *bdev, struct super_block *sb)
{
	int error = -EINVAL;

	mutex_lock(&bdev->bd_fsfreeze_mutex);
	if (!bdev->bd_fsfreeze_count)
		goto out;

	error = 0;
	if (--bdev->bd_fsfreeze_count > 0)
		goto out;

	if (!sb)
		goto out;

	error = thaw_super(sb);
	if (error) {
		bdev->bd_fsfreeze_count++;
		mutex_unlock(&bdev->bd_fsfreeze_mutex);
		return error;
	}
out:
	mutex_unlock(&bdev->bd_fsfreeze_mutex);
	return 0;
}
