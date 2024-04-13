static int insert_balance_item(struct btrfs_fs_info *fs_info,
			       struct btrfs_balance_control *bctl)
{
	struct btrfs_root *root = fs_info->tree_root;
	struct btrfs_trans_handle *trans;
	struct btrfs_balance_item *item;
	struct btrfs_disk_balance_args disk_bargs;
	struct btrfs_path *path;
	struct extent_buffer *leaf;
	struct btrfs_key key;
	int ret, err;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		btrfs_free_path(path);
		return PTR_ERR(trans);
	}

	key.objectid = BTRFS_BALANCE_OBJECTID;
	key.type = BTRFS_TEMPORARY_ITEM_KEY;
	key.offset = 0;

	ret = btrfs_insert_empty_item(trans, root, path, &key,
				      sizeof(*item));
	if (ret)
		goto out;

	leaf = path->nodes[0];
	item = btrfs_item_ptr(leaf, path->slots[0], struct btrfs_balance_item);

	memzero_extent_buffer(leaf, (unsigned long)item, sizeof(*item));

	btrfs_cpu_balance_args_to_disk(&disk_bargs, &bctl->data);
	btrfs_set_balance_data(leaf, item, &disk_bargs);
	btrfs_cpu_balance_args_to_disk(&disk_bargs, &bctl->meta);
	btrfs_set_balance_meta(leaf, item, &disk_bargs);
	btrfs_cpu_balance_args_to_disk(&disk_bargs, &bctl->sys);
	btrfs_set_balance_sys(leaf, item, &disk_bargs);

	btrfs_set_balance_flags(leaf, item, bctl->flags);

	btrfs_mark_buffer_dirty(leaf);
out:
	btrfs_free_path(path);
	err = btrfs_commit_transaction(trans);
	if (err && !ret)
		ret = err;
	return ret;
}