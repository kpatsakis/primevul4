static int update_array_info(struct mddev *mddev, mdu_array_info_t *info)
{
	int rv = 0;
	int cnt = 0;
	int state = 0;

	/* calculate expected state,ignoring low bits */
	if (mddev->bitmap && mddev->bitmap_info.offset)
		state |= (1 << MD_SB_BITMAP_PRESENT);

	if (mddev->major_version != info->major_version ||
	    mddev->minor_version != info->minor_version ||
/*	    mddev->patch_version != info->patch_version || */
	    mddev->ctime         != info->ctime         ||
	    mddev->level         != info->level         ||
/*	    mddev->layout        != info->layout        || */
	    mddev->persistent	 != !info->not_persistent ||
	    mddev->chunk_sectors != info->chunk_size >> 9 ||
	    /* ignore bottom 8 bits of state, and allow SB_BITMAP_PRESENT to change */
	    ((state^info->state) & 0xfffffe00)
		)
		return -EINVAL;
	/* Check there is only one change */
	if (info->size >= 0 && mddev->dev_sectors / 2 != info->size)
		cnt++;
	if (mddev->raid_disks != info->raid_disks)
		cnt++;
	if (mddev->layout != info->layout)
		cnt++;
	if ((state ^ info->state) & (1<<MD_SB_BITMAP_PRESENT))
		cnt++;
	if (cnt == 0)
		return 0;
	if (cnt > 1)
		return -EINVAL;

	if (mddev->layout != info->layout) {
		/* Change layout
		 * we don't need to do anything at the md level, the
		 * personality will take care of it all.
		 */
		if (mddev->pers->check_reshape == NULL)
			return -EINVAL;
		else {
			mddev->new_layout = info->layout;
			rv = mddev->pers->check_reshape(mddev);
			if (rv)
				mddev->new_layout = mddev->layout;
			return rv;
		}
	}
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_start(mddev);
	if (info->size >= 0 && mddev->dev_sectors / 2 != info->size)
		rv = update_size(mddev, (sector_t)info->size * 2);

	if (mddev->raid_disks    != info->raid_disks)
		rv = update_raid_disks(mddev, info->raid_disks);

	if ((state ^ info->state) & (1<<MD_SB_BITMAP_PRESENT)) {
		if (mddev->pers->quiesce == NULL || mddev->thread == NULL) {
			rv = -EINVAL;
			goto err;
		}
		if (mddev->recovery || mddev->sync_thread) {
			rv = -EBUSY;
			goto err;
		}
		if (info->state & (1<<MD_SB_BITMAP_PRESENT)) {
			struct bitmap *bitmap;
			/* add the bitmap */
			if (mddev->bitmap) {
				rv = -EEXIST;
				goto err;
			}
			if (mddev->bitmap_info.default_offset == 0) {
				rv = -EINVAL;
				goto err;
			}
			mddev->bitmap_info.offset =
				mddev->bitmap_info.default_offset;
			mddev->bitmap_info.space =
				mddev->bitmap_info.default_space;
			mddev->pers->quiesce(mddev, 1);
			bitmap = bitmap_create(mddev, -1);
			if (!IS_ERR(bitmap)) {
				mddev->bitmap = bitmap;
				rv = bitmap_load(mddev);
			} else
				rv = PTR_ERR(bitmap);
			if (rv)
				bitmap_destroy(mddev);
			mddev->pers->quiesce(mddev, 0);
		} else {
			/* remove the bitmap */
			if (!mddev->bitmap) {
				rv = -ENOENT;
				goto err;
			}
			if (mddev->bitmap->storage.file) {
				rv = -EINVAL;
				goto err;
			}
			mddev->pers->quiesce(mddev, 1);
			bitmap_destroy(mddev);
			mddev->pers->quiesce(mddev, 0);
			mddev->bitmap_info.offset = 0;
		}
	}
	md_update_sb(mddev, 1);
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_finish(mddev);
	return rv;
err:
	if (mddev_is_clustered(mddev))
		md_cluster_ops->metadata_update_cancel(mddev);
	return rv;
}
