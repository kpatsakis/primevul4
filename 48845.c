int truncate_blocks(struct inode *inode, u64 from)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	unsigned int blocksize = inode->i_sb->s_blocksize;
	struct dnode_of_data dn;
	pgoff_t free_from;
	int count = 0, err = 0;

	trace_f2fs_truncate_blocks_enter(inode, from);

	if (f2fs_has_inline_data(inode))
		goto done;

	free_from = (pgoff_t)
			((from + blocksize - 1) >> (sbi->log_blocksize));

	f2fs_lock_op(sbi);

	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = get_dnode_of_data(&dn, free_from, LOOKUP_NODE);
	if (err) {
		if (err == -ENOENT)
			goto free_next;
		f2fs_unlock_op(sbi);
		trace_f2fs_truncate_blocks_exit(inode, err);
		return err;
	}

	if (IS_INODE(dn.node_page))
		count = ADDRS_PER_INODE(F2FS_I(inode));
	else
		count = ADDRS_PER_BLOCK;

	count -= dn.ofs_in_node;
	f2fs_bug_on(count < 0);

	if (dn.ofs_in_node || IS_INODE(dn.node_page)) {
		truncate_data_blocks_range(&dn, count);
		free_from += count;
	}

	f2fs_put_dnode(&dn);
free_next:
	err = truncate_inode_blocks(inode, free_from);
	f2fs_unlock_op(sbi);
done:
	/* lastly zero out the first data page */
	truncate_partial_data_page(inode, from);

	trace_f2fs_truncate_blocks_exit(inode, err);
	return err;
}
