static noinline int relink_extent_backref(struct btrfs_path *path,
				 struct sa_defrag_extent_backref *prev,
				 struct sa_defrag_extent_backref *backref)
{
	struct btrfs_file_extent_item *extent;
	struct btrfs_file_extent_item *item;
	struct btrfs_ordered_extent *ordered;
	struct btrfs_trans_handle *trans;
	struct btrfs_fs_info *fs_info;
	struct btrfs_root *root;
	struct btrfs_key key;
	struct extent_buffer *leaf;
	struct old_sa_defrag_extent *old = backref->old;
	struct new_sa_defrag_extent *new = old->new;
	struct inode *src_inode = new->inode;
	struct inode *inode;
	struct extent_state *cached = NULL;
	int ret = 0;
	u64 start;
	u64 len;
	u64 lock_start;
	u64 lock_end;
	bool merge = false;
	int index;

	if (prev && prev->root_id == backref->root_id &&
	    prev->inum == backref->inum &&
	    prev->file_pos + prev->num_bytes == backref->file_pos)
		merge = true;

	/* step 1: get root */
	key.objectid = backref->root_id;
	key.type = BTRFS_ROOT_ITEM_KEY;
	key.offset = (u64)-1;

	fs_info = BTRFS_I(src_inode)->root->fs_info;
	index = srcu_read_lock(&fs_info->subvol_srcu);

	root = btrfs_read_fs_root_no_name(fs_info, &key);
	if (IS_ERR(root)) {
		srcu_read_unlock(&fs_info->subvol_srcu, index);
		if (PTR_ERR(root) == -ENOENT)
			return 0;
		return PTR_ERR(root);
	}

	if (btrfs_root_readonly(root)) {
		srcu_read_unlock(&fs_info->subvol_srcu, index);
		return 0;
	}

	/* step 2: get inode */
	key.objectid = backref->inum;
	key.type = BTRFS_INODE_ITEM_KEY;
	key.offset = 0;

	inode = btrfs_iget(fs_info->sb, &key, root, NULL);
	if (IS_ERR(inode)) {
		srcu_read_unlock(&fs_info->subvol_srcu, index);
		return 0;
	}

	srcu_read_unlock(&fs_info->subvol_srcu, index);

	/* step 3: relink backref */
	lock_start = backref->file_pos;
	lock_end = backref->file_pos + backref->num_bytes - 1;
	lock_extent_bits(&BTRFS_I(inode)->io_tree, lock_start, lock_end,
			 0, &cached);

	ordered = btrfs_lookup_first_ordered_extent(inode, lock_end);
	if (ordered) {
		btrfs_put_ordered_extent(ordered);
		goto out_unlock;
	}

	trans = btrfs_join_transaction(root);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto out_unlock;
	}

	key.objectid = backref->inum;
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = backref->file_pos;

	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0) {
		goto out_free_path;
	} else if (ret > 0) {
		ret = 0;
		goto out_free_path;
	}

	extent = btrfs_item_ptr(path->nodes[0], path->slots[0],
				struct btrfs_file_extent_item);

	if (btrfs_file_extent_generation(path->nodes[0], extent) !=
	    backref->generation)
		goto out_free_path;

	btrfs_release_path(path);

	start = backref->file_pos;
	if (backref->extent_offset < old->extent_offset + old->offset)
		start += old->extent_offset + old->offset -
			 backref->extent_offset;

	len = min(backref->extent_offset + backref->num_bytes,
		  old->extent_offset + old->offset + old->len);
	len -= max(backref->extent_offset, old->extent_offset + old->offset);

	ret = btrfs_drop_extents(trans, root, inode, start,
				 start + len, 1);
	if (ret)
		goto out_free_path;
again:
	key.objectid = btrfs_ino(inode);
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = start;

	path->leave_spinning = 1;
	if (merge) {
		struct btrfs_file_extent_item *fi;
		u64 extent_len;
		struct btrfs_key found_key;

		ret = btrfs_search_slot(trans, root, &key, path, 0, 1);
		if (ret < 0)
			goto out_free_path;

		path->slots[0]--;
		leaf = path->nodes[0];
		btrfs_item_key_to_cpu(leaf, &found_key, path->slots[0]);

		fi = btrfs_item_ptr(leaf, path->slots[0],
				    struct btrfs_file_extent_item);
		extent_len = btrfs_file_extent_num_bytes(leaf, fi);

		if (extent_len + found_key.offset == start &&
		    relink_is_mergable(leaf, fi, new)) {
			btrfs_set_file_extent_num_bytes(leaf, fi,
							extent_len + len);
			btrfs_mark_buffer_dirty(leaf);
			inode_add_bytes(inode, len);

			ret = 1;
			goto out_free_path;
		} else {
			merge = false;
			btrfs_release_path(path);
			goto again;
		}
	}

	ret = btrfs_insert_empty_item(trans, root, path, &key,
					sizeof(*extent));
	if (ret) {
		btrfs_abort_transaction(trans, root, ret);
		goto out_free_path;
	}

	leaf = path->nodes[0];
	item = btrfs_item_ptr(leaf, path->slots[0],
				struct btrfs_file_extent_item);
	btrfs_set_file_extent_disk_bytenr(leaf, item, new->bytenr);
	btrfs_set_file_extent_disk_num_bytes(leaf, item, new->disk_len);
	btrfs_set_file_extent_offset(leaf, item, start - new->file_pos);
	btrfs_set_file_extent_num_bytes(leaf, item, len);
	btrfs_set_file_extent_ram_bytes(leaf, item, new->len);
	btrfs_set_file_extent_generation(leaf, item, trans->transid);
	btrfs_set_file_extent_type(leaf, item, BTRFS_FILE_EXTENT_REG);
	btrfs_set_file_extent_compression(leaf, item, new->compress_type);
	btrfs_set_file_extent_encryption(leaf, item, 0);
	btrfs_set_file_extent_other_encoding(leaf, item, 0);

	btrfs_mark_buffer_dirty(leaf);
	inode_add_bytes(inode, len);
	btrfs_release_path(path);

	ret = btrfs_inc_extent_ref(trans, root, new->bytenr,
			new->disk_len, 0,
			backref->root_id, backref->inum,
			new->file_pos, 0);	/* start - extent_offset */
	if (ret) {
		btrfs_abort_transaction(trans, root, ret);
		goto out_free_path;
	}

	ret = 1;
out_free_path:
	btrfs_release_path(path);
	path->leave_spinning = 0;
	btrfs_end_transaction(trans, root);
out_unlock:
	unlock_extent_cached(&BTRFS_I(inode)->io_tree, lock_start, lock_end,
			     &cached, GFP_NOFS);
	iput(inode);
	return ret;
}
