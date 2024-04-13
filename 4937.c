static int __btrfs_map_block(struct btrfs_fs_info *fs_info,
			     enum btrfs_map_op op,
			     u64 logical, u64 *length,
			     struct btrfs_bio **bbio_ret,
			     int mirror_num, int need_raid_map)
{
	struct extent_map *em;
	struct map_lookup *map;
	u64 stripe_offset;
	u64 stripe_nr;
	u64 stripe_len;
	u32 stripe_index;
	int data_stripes;
	int i;
	int ret = 0;
	int num_stripes;
	int max_errors = 0;
	int tgtdev_indexes = 0;
	struct btrfs_bio *bbio = NULL;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	int dev_replace_is_ongoing = 0;
	int num_alloc_stripes;
	int patch_the_first_stripe_for_dev_replace = 0;
	u64 physical_to_patch_in_first_stripe = 0;
	u64 raid56_full_stripe_start = (u64)-1;
	struct btrfs_io_geometry geom;

	ASSERT(bbio_ret);
	ASSERT(op != BTRFS_MAP_DISCARD);

	em = btrfs_get_chunk_map(fs_info, logical, *length);
	ASSERT(!IS_ERR(em));

	ret = btrfs_get_io_geometry(fs_info, em, op, logical, &geom);
	if (ret < 0)
		return ret;

	map = em->map_lookup;

	*length = geom.len;
	stripe_len = geom.stripe_len;
	stripe_nr = geom.stripe_nr;
	stripe_offset = geom.stripe_offset;
	raid56_full_stripe_start = geom.raid56_stripe_offset;
	data_stripes = nr_data_stripes(map);

	down_read(&dev_replace->rwsem);
	dev_replace_is_ongoing = btrfs_dev_replace_is_ongoing(dev_replace);
	/*
	 * Hold the semaphore for read during the whole operation, write is
	 * requested at commit time but must wait.
	 */
	if (!dev_replace_is_ongoing)
		up_read(&dev_replace->rwsem);

	if (dev_replace_is_ongoing && mirror_num == map->num_stripes + 1 &&
	    !need_full_stripe(op) && dev_replace->tgtdev != NULL) {
		ret = get_extra_mirror_from_replace(fs_info, logical, *length,
						    dev_replace->srcdev->devid,
						    &mirror_num,
					    &physical_to_patch_in_first_stripe);
		if (ret)
			goto out;
		else
			patch_the_first_stripe_for_dev_replace = 1;
	} else if (mirror_num > map->num_stripes) {
		mirror_num = 0;
	}

	num_stripes = 1;
	stripe_index = 0;
	if (map->type & BTRFS_BLOCK_GROUP_RAID0) {
		stripe_nr = div_u64_rem(stripe_nr, map->num_stripes,
				&stripe_index);
		if (!need_full_stripe(op))
			mirror_num = 1;
	} else if (map->type & BTRFS_BLOCK_GROUP_RAID1_MASK) {
		if (need_full_stripe(op))
			num_stripes = map->num_stripes;
		else if (mirror_num)
			stripe_index = mirror_num - 1;
		else {
			stripe_index = find_live_mirror(fs_info, map, 0,
					    dev_replace_is_ongoing);
			mirror_num = stripe_index + 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_DUP) {
		if (need_full_stripe(op)) {
			num_stripes = map->num_stripes;
		} else if (mirror_num) {
			stripe_index = mirror_num - 1;
		} else {
			mirror_num = 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_RAID10) {
		u32 factor = map->num_stripes / map->sub_stripes;

		stripe_nr = div_u64_rem(stripe_nr, factor, &stripe_index);
		stripe_index *= map->sub_stripes;

		if (need_full_stripe(op))
			num_stripes = map->sub_stripes;
		else if (mirror_num)
			stripe_index += mirror_num - 1;
		else {
			int old_stripe_index = stripe_index;
			stripe_index = find_live_mirror(fs_info, map,
					      stripe_index,
					      dev_replace_is_ongoing);
			mirror_num = stripe_index - old_stripe_index + 1;
		}

	} else if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		if (need_raid_map && (need_full_stripe(op) || mirror_num > 1)) {
			/* push stripe_nr back to the start of the full stripe */
			stripe_nr = div64_u64(raid56_full_stripe_start,
					stripe_len * data_stripes);

			/* RAID[56] write or recovery. Return all stripes */
			num_stripes = map->num_stripes;
			max_errors = nr_parity_stripes(map);

			*length = map->stripe_len;
			stripe_index = 0;
			stripe_offset = 0;
		} else {
			/*
			 * Mirror #0 or #1 means the original data block.
			 * Mirror #2 is RAID5 parity block.
			 * Mirror #3 is RAID6 Q block.
			 */
			stripe_nr = div_u64_rem(stripe_nr,
					data_stripes, &stripe_index);
			if (mirror_num > 1)
				stripe_index = data_stripes + mirror_num - 2;

			/* We distribute the parity blocks across stripes */
			div_u64_rem(stripe_nr + stripe_index, map->num_stripes,
					&stripe_index);
			if (!need_full_stripe(op) && mirror_num <= 1)
				mirror_num = 1;
		}
	} else {
		/*
		 * after this, stripe_nr is the number of stripes on this
		 * device we have to walk to find the data, and stripe_index is
		 * the number of our device in the stripe array
		 */
		stripe_nr = div_u64_rem(stripe_nr, map->num_stripes,
				&stripe_index);
		mirror_num = stripe_index + 1;
	}
	if (stripe_index >= map->num_stripes) {
		btrfs_crit(fs_info,
			   "stripe index math went horribly wrong, got stripe_index=%u, num_stripes=%u",
			   stripe_index, map->num_stripes);
		ret = -EINVAL;
		goto out;
	}

	num_alloc_stripes = num_stripes;
	if (dev_replace_is_ongoing && dev_replace->tgtdev != NULL) {
		if (op == BTRFS_MAP_WRITE)
			num_alloc_stripes <<= 1;
		if (op == BTRFS_MAP_GET_READ_MIRRORS)
			num_alloc_stripes++;
		tgtdev_indexes = num_stripes;
	}

	bbio = alloc_btrfs_bio(num_alloc_stripes, tgtdev_indexes);
	if (!bbio) {
		ret = -ENOMEM;
		goto out;
	}

	for (i = 0; i < num_stripes; i++) {
		bbio->stripes[i].physical = map->stripes[stripe_index].physical +
			stripe_offset + stripe_nr * map->stripe_len;
		bbio->stripes[i].dev = map->stripes[stripe_index].dev;
		stripe_index++;
	}

	/* build raid_map */
	if (map->type & BTRFS_BLOCK_GROUP_RAID56_MASK && need_raid_map &&
	    (need_full_stripe(op) || mirror_num > 1)) {
		u64 tmp;
		unsigned rot;

		/* Work out the disk rotation on this stripe-set */
		div_u64_rem(stripe_nr, num_stripes, &rot);

		/* Fill in the logical address of each stripe */
		tmp = stripe_nr * data_stripes;
		for (i = 0; i < data_stripes; i++)
			bbio->raid_map[(i+rot) % num_stripes] =
				em->start + (tmp + i) * map->stripe_len;

		bbio->raid_map[(i+rot) % map->num_stripes] = RAID5_P_STRIPE;
		if (map->type & BTRFS_BLOCK_GROUP_RAID6)
			bbio->raid_map[(i+rot+1) % num_stripes] =
				RAID6_Q_STRIPE;

		sort_parity_stripes(bbio, num_stripes);
	}

	if (need_full_stripe(op))
		max_errors = btrfs_chunk_max_errors(map);

	if (dev_replace_is_ongoing && dev_replace->tgtdev != NULL &&
	    need_full_stripe(op)) {
		handle_ops_on_dev_replace(op, &bbio, dev_replace, logical,
					  &num_stripes, &max_errors);
	}

	*bbio_ret = bbio;
	bbio->map_type = map->type;
	bbio->num_stripes = num_stripes;
	bbio->max_errors = max_errors;
	bbio->mirror_num = mirror_num;

	/*
	 * this is the case that REQ_READ && dev_replace_is_ongoing &&
	 * mirror_num == num_stripes + 1 && dev_replace target drive is
	 * available as a mirror
	 */
	if (patch_the_first_stripe_for_dev_replace && num_stripes > 0) {
		WARN_ON(num_stripes > 1);
		bbio->stripes[0].dev = dev_replace->tgtdev;
		bbio->stripes[0].physical = physical_to_patch_in_first_stripe;
		bbio->mirror_num = map->num_stripes + 1;
	}
out:
	if (dev_replace_is_ongoing) {
		lockdep_assert_held(&dev_replace->rwsem);
		/* Unlock and let waiting writers proceed */
		up_read(&dev_replace->rwsem);
	}
	free_extent_map(em);
	return ret;
}