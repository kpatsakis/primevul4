static ssize_t recovery_start_store(struct md_rdev *rdev, const char *buf, size_t len)
{
	unsigned long long recovery_start;

	if (cmd_match(buf, "none"))
		recovery_start = MaxSector;
	else if (kstrtoull(buf, 10, &recovery_start))
		return -EINVAL;

	if (rdev->mddev->pers &&
	    rdev->raid_disk >= 0)
		return -EBUSY;

	rdev->recovery_offset = recovery_start;
	if (recovery_start == MaxSector)
		set_bit(In_sync, &rdev->flags);
	else
		clear_bit(In_sync, &rdev->flags);
	return len;
}
