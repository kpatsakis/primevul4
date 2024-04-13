static int super_1_validate(struct mddev *mddev, struct md_rdev *rdev)
{
	struct mdp_superblock_1 *sb = page_address(rdev->sb_page);
	__u64 ev1 = le64_to_cpu(sb->events);

	rdev->raid_disk = -1;
	clear_bit(Faulty, &rdev->flags);
	clear_bit(In_sync, &rdev->flags);
	clear_bit(Bitmap_sync, &rdev->flags);
	clear_bit(WriteMostly, &rdev->flags);

	if (mddev->raid_disks == 0) {
		mddev->major_version = 1;
		mddev->patch_version = 0;
		mddev->external = 0;
		mddev->chunk_sectors = le32_to_cpu(sb->chunksize);
		mddev->ctime = le64_to_cpu(sb->ctime) & ((1ULL << 32)-1);
		mddev->utime = le64_to_cpu(sb->utime) & ((1ULL << 32)-1);
		mddev->level = le32_to_cpu(sb->level);
		mddev->clevel[0] = 0;
		mddev->layout = le32_to_cpu(sb->layout);
		mddev->raid_disks = le32_to_cpu(sb->raid_disks);
		mddev->dev_sectors = le64_to_cpu(sb->size);
		mddev->events = ev1;
		mddev->bitmap_info.offset = 0;
		mddev->bitmap_info.space = 0;
		/* Default location for bitmap is 1K after superblock
		 * using 3K - total of 4K
		 */
		mddev->bitmap_info.default_offset = 1024 >> 9;
		mddev->bitmap_info.default_space = (4096-1024) >> 9;
		mddev->reshape_backwards = 0;

		mddev->recovery_cp = le64_to_cpu(sb->resync_offset);
		memcpy(mddev->uuid, sb->set_uuid, 16);

		mddev->max_disks =  (4096-256)/2;

		if ((le32_to_cpu(sb->feature_map) & MD_FEATURE_BITMAP_OFFSET) &&
		    mddev->bitmap_info.file == NULL) {
			mddev->bitmap_info.offset =
				(__s32)le32_to_cpu(sb->bitmap_offset);
			/* Metadata doesn't record how much space is available.
			 * For 1.0, we assume we can use up to the superblock
			 * if before, else to 4K beyond superblock.
			 * For others, assume no change is possible.
			 */
			if (mddev->minor_version > 0)
				mddev->bitmap_info.space = 0;
			else if (mddev->bitmap_info.offset > 0)
				mddev->bitmap_info.space =
					8 - mddev->bitmap_info.offset;
			else
				mddev->bitmap_info.space =
					-mddev->bitmap_info.offset;
		}

		if ((le32_to_cpu(sb->feature_map) & MD_FEATURE_RESHAPE_ACTIVE)) {
			mddev->reshape_position = le64_to_cpu(sb->reshape_position);
			mddev->delta_disks = le32_to_cpu(sb->delta_disks);
			mddev->new_level = le32_to_cpu(sb->new_level);
			mddev->new_layout = le32_to_cpu(sb->new_layout);
			mddev->new_chunk_sectors = le32_to_cpu(sb->new_chunk);
			if (mddev->delta_disks < 0 ||
			    (mddev->delta_disks == 0 &&
			     (le32_to_cpu(sb->feature_map)
			      & MD_FEATURE_RESHAPE_BACKWARDS)))
				mddev->reshape_backwards = 1;
		} else {
			mddev->reshape_position = MaxSector;
			mddev->delta_disks = 0;
			mddev->new_level = mddev->level;
			mddev->new_layout = mddev->layout;
			mddev->new_chunk_sectors = mddev->chunk_sectors;
		}

	} else if (mddev->pers == NULL) {
		/* Insist of good event counter while assembling, except for
		 * spares (which don't need an event count) */
		++ev1;
		if (rdev->desc_nr >= 0 &&
		    rdev->desc_nr < le32_to_cpu(sb->max_dev) &&
		    le16_to_cpu(sb->dev_roles[rdev->desc_nr]) < 0xfffe)
			if (ev1 < mddev->events)
				return -EINVAL;
	} else if (mddev->bitmap) {
		/* If adding to array with a bitmap, then we can accept an
		 * older device, but not too old.
		 */
		if (ev1 < mddev->bitmap->events_cleared)
			return 0;
		if (ev1 < mddev->events)
			set_bit(Bitmap_sync, &rdev->flags);
	} else {
		if (ev1 < mddev->events)
			/* just a hot-add of a new device, leave raid_disk at -1 */
			return 0;
	}
	if (mddev->level != LEVEL_MULTIPATH) {
		int role;
		if (rdev->desc_nr < 0 ||
		    rdev->desc_nr >= le32_to_cpu(sb->max_dev)) {
			role = 0xffff;
			rdev->desc_nr = -1;
		} else
			role = le16_to_cpu(sb->dev_roles[rdev->desc_nr]);
		switch(role) {
		case 0xffff: /* spare */
			break;
		case 0xfffe: /* faulty */
			set_bit(Faulty, &rdev->flags);
			break;
		default:
			rdev->saved_raid_disk = role;
			if ((le32_to_cpu(sb->feature_map) &
			     MD_FEATURE_RECOVERY_OFFSET)) {
				rdev->recovery_offset = le64_to_cpu(sb->recovery_offset);
				if (!(le32_to_cpu(sb->feature_map) &
				      MD_FEATURE_RECOVERY_BITMAP))
					rdev->saved_raid_disk = -1;
			} else
				set_bit(In_sync, &rdev->flags);
			rdev->raid_disk = role;
			break;
		}
		if (sb->devflags & WriteMostly1)
			set_bit(WriteMostly, &rdev->flags);
		if (le32_to_cpu(sb->feature_map) & MD_FEATURE_REPLACEMENT)
			set_bit(Replacement, &rdev->flags);
	} else /* MULTIPATH are always insync */
		set_bit(In_sync, &rdev->flags);

	return 0;
}
