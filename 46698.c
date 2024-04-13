static ssize_t new_offset_store(struct md_rdev *rdev,
				const char *buf, size_t len)
{
	unsigned long long new_offset;
	struct mddev *mddev = rdev->mddev;

	if (kstrtoull(buf, 10, &new_offset) < 0)
		return -EINVAL;

	if (mddev->sync_thread ||
	    test_bit(MD_RECOVERY_RUNNING,&mddev->recovery))
		return -EBUSY;
	if (new_offset == rdev->data_offset)
		/* reset is always permitted */
		;
	else if (new_offset > rdev->data_offset) {
		/* must not push array size beyond rdev_sectors */
		if (new_offset - rdev->data_offset
		    + mddev->dev_sectors > rdev->sectors)
				return -E2BIG;
	}
	/* Metadata worries about other space details. */

	/* decreasing the offset is inconsistent with a backwards
	 * reshape.
	 */
	if (new_offset < rdev->data_offset &&
	    mddev->reshape_backwards)
		return -EINVAL;
	/* Increasing offset is inconsistent with forwards
	 * reshape.  reshape_direction should be set to
	 * 'backwards' first.
	 */
	if (new_offset > rdev->data_offset &&
	    !mddev->reshape_backwards)
		return -EINVAL;

	if (mddev->pers && mddev->persistent &&
	    !super_types[mddev->major_version]
	    .allow_new_offset(rdev, new_offset))
		return -E2BIG;
	rdev->new_data_offset = new_offset;
	if (new_offset > rdev->data_offset)
		mddev->reshape_backwards = 1;
	else if (new_offset < rdev->data_offset)
		mddev->reshape_backwards = 0;

	return len;
}
