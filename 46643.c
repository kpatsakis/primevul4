int md_integrity_register(struct mddev *mddev)
{
	struct md_rdev *rdev, *reference = NULL;

	if (list_empty(&mddev->disks))
		return 0; /* nothing to do */
	if (!mddev->gendisk || blk_get_integrity(mddev->gendisk))
		return 0; /* shouldn't register, or already is */
	rdev_for_each(rdev, mddev) {
		/* skip spares and non-functional disks */
		if (test_bit(Faulty, &rdev->flags))
			continue;
		if (rdev->raid_disk < 0)
			continue;
		if (!reference) {
			/* Use the first rdev as the reference */
			reference = rdev;
			continue;
		}
		/* does this rdev's profile match the reference profile? */
		if (blk_integrity_compare(reference->bdev->bd_disk,
				rdev->bdev->bd_disk) < 0)
			return -EINVAL;
	}
	if (!reference || !bdev_get_integrity(reference->bdev))
		return 0;
	/*
	 * All component devices are integrity capable and have matching
	 * profiles, register the common profile for the md device.
	 */
	if (blk_integrity_register(mddev->gendisk,
			bdev_get_integrity(reference->bdev)) != 0) {
		printk(KERN_ERR "md: failed to register integrity for %s\n",
			mdname(mddev));
		return -EINVAL;
	}
	printk(KERN_NOTICE "md: data integrity enabled on %s\n", mdname(mddev));
	if (bioset_integrity_create(mddev->bio_set, BIO_POOL_SIZE)) {
		printk(KERN_ERR "md: failed to create integrity pool for %s\n",
		       mdname(mddev));
		return -EINVAL;
	}
	return 0;
}
