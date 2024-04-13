static noinline int run_delalloc_nocow(struct inode *inode,
				       struct page *locked_page,
			      u64 start, u64 end, int *page_started, int force,
			      unsigned long *nr_written)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_trans_handle *trans;
	struct extent_buffer *leaf;
	struct btrfs_path *path;
	struct btrfs_file_extent_item *fi;
	struct btrfs_key found_key;
	u64 cow_start;
	u64 cur_offset;
	u64 extent_end;
	u64 extent_offset;
	u64 disk_bytenr;
	u64 num_bytes;
	u64 disk_num_bytes;
	u64 ram_bytes;
	int extent_type;
	int ret, err;
	int type;
	int nocow;
	int check_prev = 1;
	bool nolock;
	u64 ino = btrfs_ino(inode);

	path = btrfs_alloc_path();
	if (!path) {
		extent_clear_unlock_delalloc(inode, start, end, locked_page,
					     EXTENT_LOCKED | EXTENT_DELALLOC |
					     EXTENT_DO_ACCOUNTING |
					     EXTENT_DEFRAG, PAGE_UNLOCK |
					     PAGE_CLEAR_DIRTY |
					     PAGE_SET_WRITEBACK |
					     PAGE_END_WRITEBACK);
		return -ENOMEM;
	}

	nolock = btrfs_is_free_space_inode(inode);

	if (nolock)
		trans = btrfs_join_transaction_nolock(root);
	else
		trans = btrfs_join_transaction(root);

	if (IS_ERR(trans)) {
		extent_clear_unlock_delalloc(inode, start, end, locked_page,
					     EXTENT_LOCKED | EXTENT_DELALLOC |
					     EXTENT_DO_ACCOUNTING |
					     EXTENT_DEFRAG, PAGE_UNLOCK |
					     PAGE_CLEAR_DIRTY |
					     PAGE_SET_WRITEBACK |
					     PAGE_END_WRITEBACK);
		btrfs_free_path(path);
		return PTR_ERR(trans);
	}

	trans->block_rsv = &root->fs_info->delalloc_block_rsv;

	cow_start = (u64)-1;
	cur_offset = start;
	while (1) {
		ret = btrfs_lookup_file_extent(trans, root, path, ino,
					       cur_offset, 0);
		if (ret < 0)
			goto error;
		if (ret > 0 && path->slots[0] > 0 && check_prev) {
			leaf = path->nodes[0];
			btrfs_item_key_to_cpu(leaf, &found_key,
					      path->slots[0] - 1);
			if (found_key.objectid == ino &&
			    found_key.type == BTRFS_EXTENT_DATA_KEY)
				path->slots[0]--;
		}
		check_prev = 0;
next_slot:
		leaf = path->nodes[0];
		if (path->slots[0] >= btrfs_header_nritems(leaf)) {
			ret = btrfs_next_leaf(root, path);
			if (ret < 0)
				goto error;
			if (ret > 0)
				break;
			leaf = path->nodes[0];
		}

		nocow = 0;
		disk_bytenr = 0;
		num_bytes = 0;
		btrfs_item_key_to_cpu(leaf, &found_key, path->slots[0]);

		if (found_key.objectid > ino ||
		    found_key.type > BTRFS_EXTENT_DATA_KEY ||
		    found_key.offset > end)
			break;

		if (found_key.offset > cur_offset) {
			extent_end = found_key.offset;
			extent_type = 0;
			goto out_check;
		}

		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		extent_type = btrfs_file_extent_type(leaf, fi);

		ram_bytes = btrfs_file_extent_ram_bytes(leaf, fi);
		if (extent_type == BTRFS_FILE_EXTENT_REG ||
		    extent_type == BTRFS_FILE_EXTENT_PREALLOC) {
			disk_bytenr = btrfs_file_extent_disk_bytenr(leaf, fi);
			extent_offset = btrfs_file_extent_offset(leaf, fi);
			extent_end = found_key.offset +
				btrfs_file_extent_num_bytes(leaf, fi);
			disk_num_bytes =
				btrfs_file_extent_disk_num_bytes(leaf, fi);
			if (extent_end <= start) {
				path->slots[0]++;
				goto next_slot;
			}
			if (disk_bytenr == 0)
				goto out_check;
			if (btrfs_file_extent_compression(leaf, fi) ||
			    btrfs_file_extent_encryption(leaf, fi) ||
			    btrfs_file_extent_other_encoding(leaf, fi))
				goto out_check;
			if (extent_type == BTRFS_FILE_EXTENT_REG && !force)
				goto out_check;
			if (btrfs_extent_readonly(root, disk_bytenr))
				goto out_check;
			if (btrfs_cross_ref_exist(trans, root, ino,
						  found_key.offset -
						  extent_offset, disk_bytenr))
				goto out_check;
			disk_bytenr += extent_offset;
			disk_bytenr += cur_offset - found_key.offset;
			num_bytes = min(end + 1, extent_end) - cur_offset;
			/*
			 * if there are pending snapshots for this root,
			 * we fall into common COW way.
			 */
			if (!nolock) {
				err = btrfs_start_write_no_snapshoting(root);
				if (!err)
					goto out_check;
			}
			/*
			 * force cow if csum exists in the range.
			 * this ensure that csum for a given extent are
			 * either valid or do not exist.
			 */
			if (csum_exist_in_range(root, disk_bytenr, num_bytes))
				goto out_check;
			nocow = 1;
		} else if (extent_type == BTRFS_FILE_EXTENT_INLINE) {
			extent_end = found_key.offset +
				btrfs_file_extent_inline_len(leaf,
						     path->slots[0], fi);
			extent_end = ALIGN(extent_end, root->sectorsize);
		} else {
			BUG_ON(1);
		}
out_check:
		if (extent_end <= start) {
			path->slots[0]++;
			if (!nolock && nocow)
				btrfs_end_write_no_snapshoting(root);
			goto next_slot;
		}
		if (!nocow) {
			if (cow_start == (u64)-1)
				cow_start = cur_offset;
			cur_offset = extent_end;
			if (cur_offset > end)
				break;
			path->slots[0]++;
			goto next_slot;
		}

		btrfs_release_path(path);
		if (cow_start != (u64)-1) {
			ret = cow_file_range(inode, locked_page,
					     cow_start, found_key.offset - 1,
					     page_started, nr_written, 1);
			if (ret) {
				if (!nolock && nocow)
					btrfs_end_write_no_snapshoting(root);
				goto error;
			}
			cow_start = (u64)-1;
		}

		if (extent_type == BTRFS_FILE_EXTENT_PREALLOC) {
			struct extent_map *em;
			struct extent_map_tree *em_tree;
			em_tree = &BTRFS_I(inode)->extent_tree;
			em = alloc_extent_map();
			BUG_ON(!em); /* -ENOMEM */
			em->start = cur_offset;
			em->orig_start = found_key.offset - extent_offset;
			em->len = num_bytes;
			em->block_len = num_bytes;
			em->block_start = disk_bytenr;
			em->orig_block_len = disk_num_bytes;
			em->ram_bytes = ram_bytes;
			em->bdev = root->fs_info->fs_devices->latest_bdev;
			em->mod_start = em->start;
			em->mod_len = em->len;
			set_bit(EXTENT_FLAG_PINNED, &em->flags);
			set_bit(EXTENT_FLAG_FILLING, &em->flags);
			em->generation = -1;
			while (1) {
				write_lock(&em_tree->lock);
				ret = add_extent_mapping(em_tree, em, 1);
				write_unlock(&em_tree->lock);
				if (ret != -EEXIST) {
					free_extent_map(em);
					break;
				}
				btrfs_drop_extent_cache(inode, em->start,
						em->start + em->len - 1, 0);
			}
			type = BTRFS_ORDERED_PREALLOC;
		} else {
			type = BTRFS_ORDERED_NOCOW;
		}

		ret = btrfs_add_ordered_extent(inode, cur_offset, disk_bytenr,
					       num_bytes, num_bytes, type);
		BUG_ON(ret); /* -ENOMEM */

		if (root->root_key.objectid ==
		    BTRFS_DATA_RELOC_TREE_OBJECTID) {
			ret = btrfs_reloc_clone_csums(inode, cur_offset,
						      num_bytes);
			if (ret) {
				if (!nolock && nocow)
					btrfs_end_write_no_snapshoting(root);
				goto error;
			}
		}

		extent_clear_unlock_delalloc(inode, cur_offset,
					     cur_offset + num_bytes - 1,
					     locked_page, EXTENT_LOCKED |
					     EXTENT_DELALLOC, PAGE_UNLOCK |
					     PAGE_SET_PRIVATE2);
		if (!nolock && nocow)
			btrfs_end_write_no_snapshoting(root);
		cur_offset = extent_end;
		if (cur_offset > end)
			break;
	}
	btrfs_release_path(path);

	if (cur_offset <= end && cow_start == (u64)-1) {
		cow_start = cur_offset;
		cur_offset = end;
	}

	if (cow_start != (u64)-1) {
		ret = cow_file_range(inode, locked_page, cow_start, end,
				     page_started, nr_written, 1);
		if (ret)
			goto error;
	}

error:
	err = btrfs_end_transaction(trans, root);
	if (!ret)
		ret = err;

	if (ret && cur_offset < end)
		extent_clear_unlock_delalloc(inode, cur_offset, end,
					     locked_page, EXTENT_LOCKED |
					     EXTENT_DELALLOC | EXTENT_DEFRAG |
					     EXTENT_DO_ACCOUNTING, PAGE_UNLOCK |
					     PAGE_CLEAR_DIRTY |
					     PAGE_SET_WRITEBACK |
					     PAGE_END_WRITEBACK);
	btrfs_free_path(path);
	return ret;
}
