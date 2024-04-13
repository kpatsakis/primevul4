static int __blkdev_get(struct block_device *bdev, fmode_t mode, int for_part)
{
	struct gendisk *disk;
	struct module *owner;
	int ret;
	int partno;
	int perm = 0;

	if (mode & FMODE_READ)
		perm |= MAY_READ;
	if (mode & FMODE_WRITE)
		perm |= MAY_WRITE;
	/*
	 * hooks: /n/, see "layering violations".
	 */
	if (!for_part) {
		ret = devcgroup_inode_permission(bdev->bd_inode, perm);
		if (ret != 0) {
			bdput(bdev);
			return ret;
		}
	}

 restart:

	ret = -ENXIO;
	disk = get_gendisk(bdev->bd_dev, &partno);
	if (!disk)
		goto out;
	owner = disk->fops->owner;

	disk_block_events(disk);
	mutex_lock_nested(&bdev->bd_mutex, for_part);
	if (!bdev->bd_openers) {
		bdev->bd_disk = disk;
		bdev->bd_queue = disk->queue;
		bdev->bd_contains = bdev;
		if (!partno) {
			struct backing_dev_info *bdi;

			ret = -ENXIO;
			bdev->bd_part = disk_get_part(disk, partno);
			if (!bdev->bd_part)
				goto out_clear;

			ret = 0;
			if (disk->fops->open) {
				ret = disk->fops->open(bdev, mode);
				if (ret == -ERESTARTSYS) {
					/* Lost a race with 'disk' being
					 * deleted, try again.
					 * See md.c
					 */
					disk_put_part(bdev->bd_part);
					bdev->bd_part = NULL;
					bdev->bd_disk = NULL;
					bdev->bd_queue = NULL;
					mutex_unlock(&bdev->bd_mutex);
					disk_unblock_events(disk);
					put_disk(disk);
					module_put(owner);
					goto restart;
				}
			}

			if (!ret) {
				bd_set_size(bdev,(loff_t)get_capacity(disk)<<9);
				bdi = blk_get_backing_dev_info(bdev);
				if (bdi == NULL)
					bdi = &default_backing_dev_info;
				bdev_inode_switch_bdi(bdev->bd_inode, bdi);
			}

			/*
			 * If the device is invalidated, rescan partition
			 * if open succeeded or failed with -ENOMEDIUM.
			 * The latter is necessary to prevent ghost
			 * partitions on a removed medium.
			 */
			if (bdev->bd_invalidated) {
				if (!ret)
					rescan_partitions(disk, bdev);
				else if (ret == -ENOMEDIUM)
					invalidate_partitions(disk, bdev);
			}
			if (ret)
				goto out_clear;
		} else {
			struct block_device *whole;
			whole = bdget_disk(disk, 0);
			ret = -ENOMEM;
			if (!whole)
				goto out_clear;
			BUG_ON(for_part);
			ret = __blkdev_get(whole, mode, 1);
			if (ret)
				goto out_clear;
			bdev->bd_contains = whole;
			bdev_inode_switch_bdi(bdev->bd_inode,
				whole->bd_inode->i_data.backing_dev_info);
			bdev->bd_part = disk_get_part(disk, partno);
			if (!(disk->flags & GENHD_FL_UP) ||
			    !bdev->bd_part || !bdev->bd_part->nr_sects) {
				ret = -ENXIO;
				goto out_clear;
			}
			bd_set_size(bdev, (loff_t)bdev->bd_part->nr_sects << 9);
		}
	} else {
		if (bdev->bd_contains == bdev) {
			ret = 0;
			if (bdev->bd_disk->fops->open)
				ret = bdev->bd_disk->fops->open(bdev, mode);
			/* the same as first opener case, read comment there */
			if (bdev->bd_invalidated) {
				if (!ret)
					rescan_partitions(bdev->bd_disk, bdev);
				else if (ret == -ENOMEDIUM)
					invalidate_partitions(bdev->bd_disk, bdev);
			}
			if (ret)
				goto out_unlock_bdev;
		}
		/* only one opener holds refs to the module and disk */
		put_disk(disk);
		module_put(owner);
	}
	bdev->bd_openers++;
	if (for_part)
		bdev->bd_part_count++;
	mutex_unlock(&bdev->bd_mutex);
	disk_unblock_events(disk);
	return 0;

 out_clear:
	disk_put_part(bdev->bd_part);
	bdev->bd_disk = NULL;
	bdev->bd_part = NULL;
	bdev->bd_queue = NULL;
	bdev_inode_switch_bdi(bdev->bd_inode, &default_backing_dev_info);
	if (bdev != bdev->bd_contains)
		__blkdev_put(bdev->bd_contains, mode, 1);
	bdev->bd_contains = NULL;
 out_unlock_bdev:
	mutex_unlock(&bdev->bd_mutex);
	disk_unblock_events(disk);
	put_disk(disk);
	module_put(owner);
 out:
	bdput(bdev);

	return ret;
}
