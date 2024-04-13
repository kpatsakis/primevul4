static int bind_rdev_to_array(struct md_rdev *rdev, struct mddev *mddev)
{
	char b[BDEVNAME_SIZE];
	struct kobject *ko;
	int err;

	/* prevent duplicates */
	if (find_rdev(mddev, rdev->bdev->bd_dev))
		return -EEXIST;

	/* make sure rdev->sectors exceeds mddev->dev_sectors */
	if (rdev->sectors && (mddev->dev_sectors == 0 ||
			rdev->sectors < mddev->dev_sectors)) {
		if (mddev->pers) {
			/* Cannot change size, so fail
			 * If mddev->level <= 0, then we don't care
			 * about aligning sizes (e.g. linear)
			 */
			if (mddev->level > 0)
				return -ENOSPC;
		} else
			mddev->dev_sectors = rdev->sectors;
	}

	/* Verify rdev->desc_nr is unique.
	 * If it is -1, assign a free number, else
	 * check number is not in use
	 */
	rcu_read_lock();
	if (rdev->desc_nr < 0) {
		int choice = 0;
		if (mddev->pers)
			choice = mddev->raid_disks;
		while (md_find_rdev_nr_rcu(mddev, choice))
			choice++;
		rdev->desc_nr = choice;
	} else {
		if (md_find_rdev_nr_rcu(mddev, rdev->desc_nr)) {
			rcu_read_unlock();
			return -EBUSY;
		}
	}
	rcu_read_unlock();
	if (mddev->max_disks && rdev->desc_nr >= mddev->max_disks) {
		printk(KERN_WARNING "md: %s: array is limited to %d devices\n",
		       mdname(mddev), mddev->max_disks);
		return -EBUSY;
	}
	bdevname(rdev->bdev,b);
	strreplace(b, '/', '!');

	rdev->mddev = mddev;
	printk(KERN_INFO "md: bind<%s>\n", b);

	if ((err = kobject_add(&rdev->kobj, &mddev->kobj, "dev-%s", b)))
		goto fail;

	ko = &part_to_dev(rdev->bdev->bd_part)->kobj;
	if (sysfs_create_link(&rdev->kobj, ko, "block"))
		/* failure here is OK */;
	rdev->sysfs_state = sysfs_get_dirent_safe(rdev->kobj.sd, "state");

	list_add_rcu(&rdev->same_set, &mddev->disks);
	bd_link_disk_holder(rdev->bdev, mddev->gendisk);

	/* May as well allow recovery to be retried once */
	mddev->recovery_disabled++;

	return 0;

 fail:
	printk(KERN_WARNING "md: failed to register dev-%s for %s\n",
	       b, mdname(mddev));
	return err;
}
