static int verify_one_dev_extent(struct btrfs_fs_info *fs_info,
				 u64 chunk_offset, u64 devid,
				 u64 physical_offset, u64 physical_len)
{
	struct extent_map_tree *em_tree = &fs_info->mapping_tree;
	struct extent_map *em;
	struct map_lookup *map;
	struct btrfs_device *dev;
	u64 stripe_len;
	bool found = false;
	int ret = 0;
	int i;

	read_lock(&em_tree->lock);
	em = lookup_extent_mapping(em_tree, chunk_offset, 1);
	read_unlock(&em_tree->lock);

	if (!em) {
		btrfs_err(fs_info,
"dev extent physical offset %llu on devid %llu doesn't have corresponding chunk",
			  physical_offset, devid);
		ret = -EUCLEAN;
		goto out;
	}

	map = em->map_lookup;
	stripe_len = calc_stripe_length(map->type, em->len, map->num_stripes);
	if (physical_len != stripe_len) {
		btrfs_err(fs_info,
"dev extent physical offset %llu on devid %llu length doesn't match chunk %llu, have %llu expect %llu",
			  physical_offset, devid, em->start, physical_len,
			  stripe_len);
		ret = -EUCLEAN;
		goto out;
	}

	for (i = 0; i < map->num_stripes; i++) {
		if (map->stripes[i].dev->devid == devid &&
		    map->stripes[i].physical == physical_offset) {
			found = true;
			if (map->verified_stripes >= map->num_stripes) {
				btrfs_err(fs_info,
				"too many dev extents for chunk %llu found",
					  em->start);
				ret = -EUCLEAN;
				goto out;
			}
			map->verified_stripes++;
			break;
		}
	}
	if (!found) {
		btrfs_err(fs_info,
	"dev extent physical offset %llu devid %llu has no corresponding chunk",
			physical_offset, devid);
		ret = -EUCLEAN;
	}

	/* Make sure no dev extent is beyond device boundary */
	dev = btrfs_find_device(fs_info->fs_devices, devid, NULL, NULL);
	if (!dev) {
		btrfs_err(fs_info, "failed to find devid %llu", devid);
		ret = -EUCLEAN;
		goto out;
	}

	if (physical_offset + physical_len > dev->disk_total_bytes) {
		btrfs_err(fs_info,
"dev extent devid %llu physical offset %llu len %llu is beyond device boundary %llu",
			  devid, physical_offset, physical_len,
			  dev->disk_total_bytes);
		ret = -EUCLEAN;
		goto out;
	}

	if (dev->zone_info) {
		u64 zone_size = dev->zone_info->zone_size;

		if (!IS_ALIGNED(physical_offset, zone_size) ||
		    !IS_ALIGNED(physical_len, zone_size)) {
			btrfs_err(fs_info,
"zoned: dev extent devid %llu physical offset %llu len %llu is not aligned to device zone",
				  devid, physical_offset, physical_len);
			ret = -EUCLEAN;
			goto out;
		}
	}

out:
	free_extent_map(em);
	return ret;
}