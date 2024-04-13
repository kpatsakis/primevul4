static struct btrfs_block_group *create_chunk(struct btrfs_trans_handle *trans,
			struct alloc_chunk_ctl *ctl,
			struct btrfs_device_info *devices_info)
{
	struct btrfs_fs_info *info = trans->fs_info;
	struct map_lookup *map = NULL;
	struct extent_map_tree *em_tree;
	struct btrfs_block_group *block_group;
	struct extent_map *em;
	u64 start = ctl->start;
	u64 type = ctl->type;
	int ret;
	int i;
	int j;

	map = kmalloc(map_lookup_size(ctl->num_stripes), GFP_NOFS);
	if (!map)
		return ERR_PTR(-ENOMEM);
	map->num_stripes = ctl->num_stripes;

	for (i = 0; i < ctl->ndevs; ++i) {
		for (j = 0; j < ctl->dev_stripes; ++j) {
			int s = i * ctl->dev_stripes + j;
			map->stripes[s].dev = devices_info[i].dev;
			map->stripes[s].physical = devices_info[i].dev_offset +
						   j * ctl->stripe_size;
		}
	}
	map->stripe_len = BTRFS_STRIPE_LEN;
	map->io_align = BTRFS_STRIPE_LEN;
	map->io_width = BTRFS_STRIPE_LEN;
	map->type = type;
	map->sub_stripes = ctl->sub_stripes;

	trace_btrfs_chunk_alloc(info, map, start, ctl->chunk_size);

	em = alloc_extent_map();
	if (!em) {
		kfree(map);
		return ERR_PTR(-ENOMEM);
	}
	set_bit(EXTENT_FLAG_FS_MAPPING, &em->flags);
	em->map_lookup = map;
	em->start = start;
	em->len = ctl->chunk_size;
	em->block_start = 0;
	em->block_len = em->len;
	em->orig_block_len = ctl->stripe_size;

	em_tree = &info->mapping_tree;
	write_lock(&em_tree->lock);
	ret = add_extent_mapping(em_tree, em, 0);
	if (ret) {
		write_unlock(&em_tree->lock);
		free_extent_map(em);
		return ERR_PTR(ret);
	}
	write_unlock(&em_tree->lock);

	block_group = btrfs_make_block_group(trans, 0, type, start, ctl->chunk_size);
	if (IS_ERR(block_group))
		goto error_del_extent;

	for (i = 0; i < map->num_stripes; i++) {
		struct btrfs_device *dev = map->stripes[i].dev;

		btrfs_device_set_bytes_used(dev,
					    dev->bytes_used + ctl->stripe_size);
		if (list_empty(&dev->post_commit_list))
			list_add_tail(&dev->post_commit_list,
				      &trans->transaction->dev_update_list);
	}

	atomic64_sub(ctl->stripe_size * map->num_stripes,
		     &info->free_chunk_space);

	free_extent_map(em);
	check_raid56_incompat_flag(info, type);
	check_raid1c34_incompat_flag(info, type);

	return block_group;

error_del_extent:
	write_lock(&em_tree->lock);
	remove_extent_mapping(em_tree, em);
	write_unlock(&em_tree->lock);

	/* One for our allocation */
	free_extent_map(em);
	/* One for the tree reference */
	free_extent_map(em);

	return block_group;
}