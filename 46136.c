static void btrfs_clear_bit_hook(struct inode *inode,
				 struct extent_state *state,
				 unsigned *bits)
{
	u64 len = state->end + 1 - state->start;
	u64 num_extents = div64_u64(len + BTRFS_MAX_EXTENT_SIZE -1,
				    BTRFS_MAX_EXTENT_SIZE);

	spin_lock(&BTRFS_I(inode)->lock);
	if ((state->state & EXTENT_DEFRAG) && (*bits & EXTENT_DEFRAG))
		BTRFS_I(inode)->defrag_bytes -= len;
	spin_unlock(&BTRFS_I(inode)->lock);

	/*
	 * set_bit and clear bit hooks normally require _irqsave/restore
	 * but in this case, we are only testing for the DELALLOC
	 * bit, which is only set or cleared with irqs on
	 */
	if ((state->state & EXTENT_DELALLOC) && (*bits & EXTENT_DELALLOC)) {
		struct btrfs_root *root = BTRFS_I(inode)->root;
		bool do_list = !btrfs_is_free_space_inode(inode);

		if (*bits & EXTENT_FIRST_DELALLOC) {
			*bits &= ~EXTENT_FIRST_DELALLOC;
		} else if (!(*bits & EXTENT_DO_ACCOUNTING)) {
			spin_lock(&BTRFS_I(inode)->lock);
			BTRFS_I(inode)->outstanding_extents -= num_extents;
			spin_unlock(&BTRFS_I(inode)->lock);
		}

		/*
		 * We don't reserve metadata space for space cache inodes so we
		 * don't need to call dellalloc_release_metadata if there is an
		 * error.
		 */
		if (*bits & EXTENT_DO_ACCOUNTING &&
		    root != root->fs_info->tree_root)
			btrfs_delalloc_release_metadata(inode, len);

		/* For sanity tests. */
		if (btrfs_test_is_dummy_root(root))
			return;

		if (root->root_key.objectid != BTRFS_DATA_RELOC_TREE_OBJECTID
		    && do_list && !(state->state & EXTENT_NORESERVE))
			btrfs_free_reserved_data_space(inode, len);

		__percpu_counter_add(&root->fs_info->delalloc_bytes, -len,
				     root->fs_info->delalloc_batch);
		spin_lock(&BTRFS_I(inode)->lock);
		BTRFS_I(inode)->delalloc_bytes -= len;
		if (do_list && BTRFS_I(inode)->delalloc_bytes == 0 &&
		    test_bit(BTRFS_INODE_IN_DELALLOC_LIST,
			     &BTRFS_I(inode)->runtime_flags))
			btrfs_del_delalloc_inode(root, inode);
		spin_unlock(&BTRFS_I(inode)->lock);
	}
}
