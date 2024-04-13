record_old_file_extents(struct inode *inode,
			struct btrfs_ordered_extent *ordered)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct old_sa_defrag_extent *old;
	struct new_sa_defrag_extent *new;
	int ret;

	new = kmalloc(sizeof(*new), GFP_NOFS);
	if (!new)
		return NULL;

	new->inode = inode;
	new->file_pos = ordered->file_offset;
	new->len = ordered->len;
	new->bytenr = ordered->start;
	new->disk_len = ordered->disk_len;
	new->compress_type = ordered->compress_type;
	new->root = RB_ROOT;
	INIT_LIST_HEAD(&new->head);

	path = btrfs_alloc_path();
	if (!path)
		goto out_kfree;

	key.objectid = btrfs_ino(inode);
	key.type = BTRFS_EXTENT_DATA_KEY;
	key.offset = new->file_pos;

	ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
	if (ret < 0)
		goto out_free_path;
	if (ret > 0 && path->slots[0] > 0)
		path->slots[0]--;

	/* find out all the old extents for the file range */
	while (1) {
		struct btrfs_file_extent_item *extent;
		struct extent_buffer *l;
		int slot;
		u64 num_bytes;
		u64 offset;
		u64 end;
		u64 disk_bytenr;
		u64 extent_offset;

		l = path->nodes[0];
		slot = path->slots[0];

		if (slot >= btrfs_header_nritems(l)) {
			ret = btrfs_next_leaf(root, path);
			if (ret < 0)
				goto out_free_path;
			else if (ret > 0)
				break;
			continue;
		}

		btrfs_item_key_to_cpu(l, &key, slot);

		if (key.objectid != btrfs_ino(inode))
			break;
		if (key.type != BTRFS_EXTENT_DATA_KEY)
			break;
		if (key.offset >= new->file_pos + new->len)
			break;

		extent = btrfs_item_ptr(l, slot, struct btrfs_file_extent_item);

		num_bytes = btrfs_file_extent_num_bytes(l, extent);
		if (key.offset + num_bytes < new->file_pos)
			goto next;

		disk_bytenr = btrfs_file_extent_disk_bytenr(l, extent);
		if (!disk_bytenr)
			goto next;

		extent_offset = btrfs_file_extent_offset(l, extent);

		old = kmalloc(sizeof(*old), GFP_NOFS);
		if (!old)
			goto out_free_path;

		offset = max(new->file_pos, key.offset);
		end = min(new->file_pos + new->len, key.offset + num_bytes);

		old->bytenr = disk_bytenr;
		old->extent_offset = extent_offset;
		old->offset = offset - key.offset;
		old->len = end - offset;
		old->new = new;
		old->count = 0;
		list_add_tail(&old->list, &new->head);
next:
		path->slots[0]++;
		cond_resched();
	}

	btrfs_free_path(path);
	atomic_inc(&root->fs_info->defrag_running);

	return new;

out_free_path:
	btrfs_free_path(path);
out_kfree:
	free_sa_defrag_extent(new);
	return NULL;
}
