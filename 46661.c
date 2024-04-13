int md_run(struct mddev *mddev)
{
	int err;
	struct md_rdev *rdev;
	struct md_personality *pers;

	if (list_empty(&mddev->disks))
		/* cannot run an array with no devices.. */
		return -EINVAL;

	if (mddev->pers)
		return -EBUSY;
	/* Cannot run until previous stop completes properly */
	if (mddev->sysfs_active)
		return -EBUSY;

	/*
	 * Analyze all RAID superblock(s)
	 */
	if (!mddev->raid_disks) {
		if (!mddev->persistent)
			return -EINVAL;
		analyze_sbs(mddev);
	}

	if (mddev->level != LEVEL_NONE)
		request_module("md-level-%d", mddev->level);
	else if (mddev->clevel[0])
		request_module("md-%s", mddev->clevel);

	/*
	 * Drop all container device buffers, from now on
	 * the only valid external interface is through the md
	 * device.
	 */
	rdev_for_each(rdev, mddev) {
		if (test_bit(Faulty, &rdev->flags))
			continue;
		sync_blockdev(rdev->bdev);
		invalidate_bdev(rdev->bdev);

		/* perform some consistency tests on the device.
		 * We don't want the data to overlap the metadata,
		 * Internal Bitmap issues have been handled elsewhere.
		 */
		if (rdev->meta_bdev) {
			/* Nothing to check */;
		} else if (rdev->data_offset < rdev->sb_start) {
			if (mddev->dev_sectors &&
			    rdev->data_offset + mddev->dev_sectors
			    > rdev->sb_start) {
				printk("md: %s: data overlaps metadata\n",
				       mdname(mddev));
				return -EINVAL;
			}
		} else {
			if (rdev->sb_start + rdev->sb_size/512
			    > rdev->data_offset) {
				printk("md: %s: metadata overlaps data\n",
				       mdname(mddev));
				return -EINVAL;
			}
		}
		sysfs_notify_dirent_safe(rdev->sysfs_state);
	}

	if (mddev->bio_set == NULL)
		mddev->bio_set = bioset_create(BIO_POOL_SIZE, 0);

	spin_lock(&pers_lock);
	pers = find_pers(mddev->level, mddev->clevel);
	if (!pers || !try_module_get(pers->owner)) {
		spin_unlock(&pers_lock);
		if (mddev->level != LEVEL_NONE)
			printk(KERN_WARNING "md: personality for level %d is not loaded!\n",
			       mddev->level);
		else
			printk(KERN_WARNING "md: personality for level %s is not loaded!\n",
			       mddev->clevel);
		return -EINVAL;
	}
	spin_unlock(&pers_lock);
	if (mddev->level != pers->level) {
		mddev->level = pers->level;
		mddev->new_level = pers->level;
	}
	strlcpy(mddev->clevel, pers->name, sizeof(mddev->clevel));

	if (mddev->reshape_position != MaxSector &&
	    pers->start_reshape == NULL) {
		/* This personality cannot handle reshaping... */
		module_put(pers->owner);
		return -EINVAL;
	}

	if (pers->sync_request) {
		/* Warn if this is a potentially silly
		 * configuration.
		 */
		char b[BDEVNAME_SIZE], b2[BDEVNAME_SIZE];
		struct md_rdev *rdev2;
		int warned = 0;

		rdev_for_each(rdev, mddev)
			rdev_for_each(rdev2, mddev) {
				if (rdev < rdev2 &&
				    rdev->bdev->bd_contains ==
				    rdev2->bdev->bd_contains) {
					printk(KERN_WARNING
					       "%s: WARNING: %s appears to be"
					       " on the same physical disk as"
					       " %s.\n",
					       mdname(mddev),
					       bdevname(rdev->bdev,b),
					       bdevname(rdev2->bdev,b2));
					warned = 1;
				}
			}

		if (warned)
			printk(KERN_WARNING
			       "True protection against single-disk"
			       " failure might be compromised.\n");
	}

	mddev->recovery = 0;
	/* may be over-ridden by personality */
	mddev->resync_max_sectors = mddev->dev_sectors;

	mddev->ok_start_degraded = start_dirty_degraded;

	if (start_readonly && mddev->ro == 0)
		mddev->ro = 2; /* read-only, but switch on first write */

	err = pers->run(mddev);
	if (err)
		printk(KERN_ERR "md: pers->run() failed ...\n");
	else if (pers->size(mddev, 0, 0) < mddev->array_sectors) {
		WARN_ONCE(!mddev->external_size, "%s: default size too small,"
			  " but 'external_size' not in effect?\n", __func__);
		printk(KERN_ERR
		       "md: invalid array_size %llu > default size %llu\n",
		       (unsigned long long)mddev->array_sectors / 2,
		       (unsigned long long)pers->size(mddev, 0, 0) / 2);
		err = -EINVAL;
	}
	if (err == 0 && pers->sync_request &&
	    (mddev->bitmap_info.file || mddev->bitmap_info.offset)) {
		struct bitmap *bitmap;

		bitmap = bitmap_create(mddev, -1);
		if (IS_ERR(bitmap)) {
			err = PTR_ERR(bitmap);
			printk(KERN_ERR "%s: failed to create bitmap (%d)\n",
			       mdname(mddev), err);
		} else
			mddev->bitmap = bitmap;

	}
	if (err) {
		mddev_detach(mddev);
		if (mddev->private)
			pers->free(mddev, mddev->private);
		mddev->private = NULL;
		module_put(pers->owner);
		bitmap_destroy(mddev);
		return err;
	}
	if (mddev->queue) {
		mddev->queue->backing_dev_info.congested_data = mddev;
		mddev->queue->backing_dev_info.congested_fn = md_congested;
		blk_queue_merge_bvec(mddev->queue, md_mergeable_bvec);
	}
	if (pers->sync_request) {
		if (mddev->kobj.sd &&
		    sysfs_create_group(&mddev->kobj, &md_redundancy_group))
			printk(KERN_WARNING
			       "md: cannot register extra attributes for %s\n",
			       mdname(mddev));
		mddev->sysfs_action = sysfs_get_dirent_safe(mddev->kobj.sd, "sync_action");
	} else if (mddev->ro == 2) /* auto-readonly not meaningful */
		mddev->ro = 0;

	atomic_set(&mddev->writes_pending,0);
	atomic_set(&mddev->max_corr_read_errors,
		   MD_DEFAULT_MAX_CORRECTED_READ_ERRORS);
	mddev->safemode = 0;
	mddev->safemode_timer.function = md_safemode_timeout;
	mddev->safemode_timer.data = (unsigned long) mddev;
	mddev->safemode_delay = (200 * HZ)/1000 +1; /* 200 msec delay */
	mddev->in_sync = 1;
	smp_wmb();
	spin_lock(&mddev->lock);
	mddev->pers = pers;
	mddev->ready = 1;
	spin_unlock(&mddev->lock);
	rdev_for_each(rdev, mddev)
		if (rdev->raid_disk >= 0)
			if (sysfs_link_rdev(mddev, rdev))
				/* failure here is OK */;

	set_bit(MD_RECOVERY_NEEDED, &mddev->recovery);

	if (mddev->flags & MD_UPDATE_SB_FLAGS)
		md_update_sb(mddev, 0);

	md_new_event(mddev);
	sysfs_notify_dirent_safe(mddev->sysfs_state);
	sysfs_notify_dirent_safe(mddev->sysfs_action);
	sysfs_notify(&mddev->kobj, NULL, "degraded");
	return 0;
}
