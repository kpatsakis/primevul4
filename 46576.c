static int add_new_disk(struct mddev *mddev, mdu_disk_info_t *info)
{
	char b[BDEVNAME_SIZE], b2[BDEVNAME_SIZE];
	struct md_rdev *rdev;
	dev_t dev = MKDEV(info->major,info->minor);

	if (mddev_is_clustered(mddev) &&
		!(info->state & ((1 << MD_DISK_CLUSTER_ADD) | (1 << MD_DISK_CANDIDATE)))) {
		pr_err("%s: Cannot add to clustered mddev.\n",
			       mdname(mddev));
		return -EINVAL;
	}

	if (info->major != MAJOR(dev) || info->minor != MINOR(dev))
		return -EOVERFLOW;

	if (!mddev->raid_disks) {
		int err;
		/* expecting a device which has a superblock */
		rdev = md_import_device(dev, mddev->major_version, mddev->minor_version);
		if (IS_ERR(rdev)) {
			printk(KERN_WARNING
				"md: md_import_device returned %ld\n",
				PTR_ERR(rdev));
			return PTR_ERR(rdev);
		}
		if (!list_empty(&mddev->disks)) {
			struct md_rdev *rdev0
				= list_entry(mddev->disks.next,
					     struct md_rdev, same_set);
			err = super_types[mddev->major_version]
				.load_super(rdev, rdev0, mddev->minor_version);
			if (err < 0) {
				printk(KERN_WARNING
					"md: %s has different UUID to %s\n",
					bdevname(rdev->bdev,b),
					bdevname(rdev0->bdev,b2));
				export_rdev(rdev);
				return -EINVAL;
			}
		}
		err = bind_rdev_to_array(rdev, mddev);
		if (err)
			export_rdev(rdev);
		return err;
	}

	/*
	 * add_new_disk can be used once the array is assembled
	 * to add "hot spares".  They must already have a superblock
	 * written
	 */
	if (mddev->pers) {
		int err;
		if (!mddev->pers->hot_add_disk) {
			printk(KERN_WARNING
				"%s: personality does not support diskops!\n",
			       mdname(mddev));
			return -EINVAL;
		}
		if (mddev->persistent)
			rdev = md_import_device(dev, mddev->major_version,
						mddev->minor_version);
		else
			rdev = md_import_device(dev, -1, -1);
		if (IS_ERR(rdev)) {
			printk(KERN_WARNING
				"md: md_import_device returned %ld\n",
				PTR_ERR(rdev));
			return PTR_ERR(rdev);
		}
		/* set saved_raid_disk if appropriate */
		if (!mddev->persistent) {
			if (info->state & (1<<MD_DISK_SYNC)  &&
			    info->raid_disk < mddev->raid_disks) {
				rdev->raid_disk = info->raid_disk;
				set_bit(In_sync, &rdev->flags);
				clear_bit(Bitmap_sync, &rdev->flags);
			} else
				rdev->raid_disk = -1;
			rdev->saved_raid_disk = rdev->raid_disk;
		} else
			super_types[mddev->major_version].
				validate_super(mddev, rdev);
		if ((info->state & (1<<MD_DISK_SYNC)) &&
		     rdev->raid_disk != info->raid_disk) {
			/* This was a hot-add request, but events doesn't
			 * match, so reject it.
			 */
			export_rdev(rdev);
			return -EINVAL;
		}

		clear_bit(In_sync, &rdev->flags); /* just to be sure */
		if (info->state & (1<<MD_DISK_WRITEMOSTLY))
			set_bit(WriteMostly, &rdev->flags);
		else
			clear_bit(WriteMostly, &rdev->flags);

		/*
		 * check whether the device shows up in other nodes
		 */
		if (mddev_is_clustered(mddev)) {
			if (info->state & (1 << MD_DISK_CANDIDATE)) {
				/* Through --cluster-confirm */
				set_bit(Candidate, &rdev->flags);
				err = md_cluster_ops->new_disk_ack(mddev, true);
				if (err) {
					export_rdev(rdev);
					return err;
				}
			} else if (info->state & (1 << MD_DISK_CLUSTER_ADD)) {
				/* --add initiated by this node */
				err = md_cluster_ops->add_new_disk_start(mddev, rdev);
				if (err) {
					md_cluster_ops->add_new_disk_finish(mddev);
					export_rdev(rdev);
					return err;
				}
			}
		}

		rdev->raid_disk = -1;
		err = bind_rdev_to_array(rdev, mddev);
		if (err)
			export_rdev(rdev);
		else
			err = add_bound_rdev(rdev);
		if (mddev_is_clustered(mddev) &&
				(info->state & (1 << MD_DISK_CLUSTER_ADD)))
			md_cluster_ops->add_new_disk_finish(mddev);
		return err;
	}

	/* otherwise, add_new_disk is only allowed
	 * for major_version==0 superblocks
	 */
	if (mddev->major_version != 0) {
		printk(KERN_WARNING "%s: ADD_NEW_DISK not supported\n",
		       mdname(mddev));
		return -EINVAL;
	}

	if (!(info->state & (1<<MD_DISK_FAULTY))) {
		int err;
		rdev = md_import_device(dev, -1, 0);
		if (IS_ERR(rdev)) {
			printk(KERN_WARNING
				"md: error, md_import_device() returned %ld\n",
				PTR_ERR(rdev));
			return PTR_ERR(rdev);
		}
		rdev->desc_nr = info->number;
		if (info->raid_disk < mddev->raid_disks)
			rdev->raid_disk = info->raid_disk;
		else
			rdev->raid_disk = -1;

		if (rdev->raid_disk < mddev->raid_disks)
			if (info->state & (1<<MD_DISK_SYNC))
				set_bit(In_sync, &rdev->flags);

		if (info->state & (1<<MD_DISK_WRITEMOSTLY))
			set_bit(WriteMostly, &rdev->flags);

		if (!mddev->persistent) {
			printk(KERN_INFO "md: nonpersistent superblock ...\n");
			rdev->sb_start = i_size_read(rdev->bdev->bd_inode) / 512;
		} else
			rdev->sb_start = calc_dev_sboffset(rdev);
		rdev->sectors = rdev->sb_start;

		err = bind_rdev_to_array(rdev, mddev);
		if (err) {
			export_rdev(rdev);
			return err;
		}
	}

	return 0;
}
