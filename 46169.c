static struct inode *btrfs_new_inode(struct btrfs_trans_handle *trans,
				     struct btrfs_root *root,
				     struct inode *dir,
				     const char *name, int name_len,
				     u64 ref_objectid, u64 objectid,
				     umode_t mode, u64 *index)
{
	struct inode *inode;
	struct btrfs_inode_item *inode_item;
	struct btrfs_key *location;
	struct btrfs_path *path;
	struct btrfs_inode_ref *ref;
	struct btrfs_key key[2];
	u32 sizes[2];
	int nitems = name ? 2 : 1;
	unsigned long ptr;
	int ret;

	path = btrfs_alloc_path();
	if (!path)
		return ERR_PTR(-ENOMEM);

	inode = new_inode(root->fs_info->sb);
	if (!inode) {
		btrfs_free_path(path);
		return ERR_PTR(-ENOMEM);
	}

	/*
	 * O_TMPFILE, set link count to 0, so that after this point,
	 * we fill in an inode item with the correct link count.
	 */
	if (!name)
		set_nlink(inode, 0);

	/*
	 * we have to initialize this early, so we can reclaim the inode
	 * number if we fail afterwards in this function.
	 */
	inode->i_ino = objectid;

	if (dir && name) {
		trace_btrfs_inode_request(dir);

		ret = btrfs_set_inode_index(dir, index);
		if (ret) {
			btrfs_free_path(path);
			iput(inode);
			return ERR_PTR(ret);
		}
	} else if (dir) {
		*index = 0;
	}
	/*
	 * index_cnt is ignored for everything but a dir,
	 * btrfs_get_inode_index_count has an explanation for the magic
	 * number
	 */
	BTRFS_I(inode)->index_cnt = 2;
	BTRFS_I(inode)->dir_index = *index;
	BTRFS_I(inode)->root = root;
	BTRFS_I(inode)->generation = trans->transid;
	inode->i_generation = BTRFS_I(inode)->generation;

	/*
	 * We could have gotten an inode number from somebody who was fsynced
	 * and then removed in this same transaction, so let's just set full
	 * sync since it will be a full sync anyway and this will blow away the
	 * old info in the log.
	 */
	set_bit(BTRFS_INODE_NEEDS_FULL_SYNC, &BTRFS_I(inode)->runtime_flags);

	key[0].objectid = objectid;
	key[0].type = BTRFS_INODE_ITEM_KEY;
	key[0].offset = 0;

	sizes[0] = sizeof(struct btrfs_inode_item);

	if (name) {
		/*
		 * Start new inodes with an inode_ref. This is slightly more
		 * efficient for small numbers of hard links since they will
		 * be packed into one item. Extended refs will kick in if we
		 * add more hard links than can fit in the ref item.
		 */
		key[1].objectid = objectid;
		key[1].type = BTRFS_INODE_REF_KEY;
		key[1].offset = ref_objectid;

		sizes[1] = name_len + sizeof(*ref);
	}

	location = &BTRFS_I(inode)->location;
	location->objectid = objectid;
	location->offset = 0;
	location->type = BTRFS_INODE_ITEM_KEY;

	ret = btrfs_insert_inode_locked(inode);
	if (ret < 0)
		goto fail;

	path->leave_spinning = 1;
	ret = btrfs_insert_empty_items(trans, root, path, key, sizes, nitems);
	if (ret != 0)
		goto fail_unlock;

	inode_init_owner(inode, dir, mode);
	inode_set_bytes(inode, 0);

	inode->i_mtime = CURRENT_TIME;
	inode->i_atime = inode->i_mtime;
	inode->i_ctime = inode->i_mtime;
	BTRFS_I(inode)->i_otime = inode->i_mtime;

	inode_item = btrfs_item_ptr(path->nodes[0], path->slots[0],
				  struct btrfs_inode_item);
	memset_extent_buffer(path->nodes[0], 0, (unsigned long)inode_item,
			     sizeof(*inode_item));
	fill_inode_item(trans, path->nodes[0], inode_item, inode);

	if (name) {
		ref = btrfs_item_ptr(path->nodes[0], path->slots[0] + 1,
				     struct btrfs_inode_ref);
		btrfs_set_inode_ref_name_len(path->nodes[0], ref, name_len);
		btrfs_set_inode_ref_index(path->nodes[0], ref, *index);
		ptr = (unsigned long)(ref + 1);
		write_extent_buffer(path->nodes[0], name, ptr, name_len);
	}

	btrfs_mark_buffer_dirty(path->nodes[0]);
	btrfs_free_path(path);

	btrfs_inherit_iflags(inode, dir);

	if (S_ISREG(mode)) {
		if (btrfs_test_opt(root, NODATASUM))
			BTRFS_I(inode)->flags |= BTRFS_INODE_NODATASUM;
		if (btrfs_test_opt(root, NODATACOW))
			BTRFS_I(inode)->flags |= BTRFS_INODE_NODATACOW |
				BTRFS_INODE_NODATASUM;
	}

	inode_tree_add(inode);

	trace_btrfs_inode_new(inode);
	btrfs_set_inode_last_trans(trans, inode);

	btrfs_update_root_times(trans, root);

	ret = btrfs_inode_inherit_props(trans, inode, dir);
	if (ret)
		btrfs_err(root->fs_info,
			  "error inheriting props for ino %llu (root %llu): %d",
			  btrfs_ino(inode), root->root_key.objectid, ret);

	return inode;

fail_unlock:
	unlock_new_inode(inode);
fail:
	if (dir && name)
		BTRFS_I(dir)->index_cnt--;
	btrfs_free_path(path);
	iput(inode);
	return ERR_PTR(ret);
}
