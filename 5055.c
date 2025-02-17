static int read_one_chunk(struct btrfs_key *key, struct extent_buffer *leaf,
			  struct btrfs_chunk *chunk)
{
	struct btrfs_fs_info *fs_info = leaf->fs_info;
	struct extent_map_tree *map_tree = &fs_info->mapping_tree;
	struct map_lookup *map;
	struct extent_map *em;
	u64 logical;
	u64 length;
	u64 devid;
	u64 type;
	u8 uuid[BTRFS_UUID_SIZE];
	int num_stripes;
	int ret;
	int i;

	logical = key->offset;
	length = btrfs_chunk_length(leaf, chunk);
	type = btrfs_chunk_type(leaf, chunk);
	num_stripes = btrfs_chunk_num_stripes(leaf, chunk);

#if BITS_PER_LONG == 32
	ret = check_32bit_meta_chunk(fs_info, logical, length, type);
	if (ret < 0)
		return ret;
	warn_32bit_meta_chunk(fs_info, logical, length, type);
#endif

	/*
	 * Only need to verify chunk item if we're reading from sys chunk array,
	 * as chunk item in tree block is already verified by tree-checker.
	 */
	if (leaf->start == BTRFS_SUPER_INFO_OFFSET) {
		ret = btrfs_check_chunk_valid(leaf, chunk, logical);
		if (ret)
			return ret;
	}

	read_lock(&map_tree->lock);
	em = lookup_extent_mapping(map_tree, logical, 1);
	read_unlock(&map_tree->lock);

	/* already mapped? */
	if (em && em->start <= logical && em->start + em->len > logical) {
		free_extent_map(em);
		return 0;
	} else if (em) {
		free_extent_map(em);
	}

	em = alloc_extent_map();
	if (!em)
		return -ENOMEM;
	map = kmalloc(map_lookup_size(num_stripes), GFP_NOFS);
	if (!map) {
		free_extent_map(em);
		return -ENOMEM;
	}

	set_bit(EXTENT_FLAG_FS_MAPPING, &em->flags);
	em->map_lookup = map;
	em->start = logical;
	em->len = length;
	em->orig_start = 0;
	em->block_start = 0;
	em->block_len = em->len;

	map->num_stripes = num_stripes;
	map->io_width = btrfs_chunk_io_width(leaf, chunk);
	map->io_align = btrfs_chunk_io_align(leaf, chunk);
	map->stripe_len = btrfs_chunk_stripe_len(leaf, chunk);
	map->type = type;
	map->sub_stripes = btrfs_chunk_sub_stripes(leaf, chunk);
	map->verified_stripes = 0;
	em->orig_block_len = calc_stripe_length(type, em->len,
						map->num_stripes);
	for (i = 0; i < num_stripes; i++) {
		map->stripes[i].physical =
			btrfs_stripe_offset_nr(leaf, chunk, i);
		devid = btrfs_stripe_devid_nr(leaf, chunk, i);
		read_extent_buffer(leaf, uuid, (unsigned long)
				   btrfs_stripe_dev_uuid_nr(chunk, i),
				   BTRFS_UUID_SIZE);
		map->stripes[i].dev = btrfs_find_device(fs_info->fs_devices,
							devid, uuid, NULL);
		if (!map->stripes[i].dev &&
		    !btrfs_test_opt(fs_info, DEGRADED)) {
			free_extent_map(em);
			btrfs_report_missing_device(fs_info, devid, uuid, true);
			return -ENOENT;
		}
		if (!map->stripes[i].dev) {
			map->stripes[i].dev =
				add_missing_dev(fs_info->fs_devices, devid,
						uuid);
			if (IS_ERR(map->stripes[i].dev)) {
				free_extent_map(em);
				btrfs_err(fs_info,
					"failed to init missing dev %llu: %ld",
					devid, PTR_ERR(map->stripes[i].dev));
				return PTR_ERR(map->stripes[i].dev);
			}
			btrfs_report_missing_device(fs_info, devid, uuid, false);
		}
		set_bit(BTRFS_DEV_STATE_IN_FS_METADATA,
				&(map->stripes[i].dev->dev_state));

	}

	write_lock(&map_tree->lock);
	ret = add_extent_mapping(map_tree, em, 0);
	write_unlock(&map_tree->lock);
	if (ret < 0) {
		btrfs_err(fs_info,
			  "failed to add chunk map, start=%llu len=%llu: %d",
			  em->start, em->len, ret);
	}
	free_extent_map(em);

	return ret;
}