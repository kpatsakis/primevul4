static struct buffer_head *udf_getblk(struct inode *inode, long block,
				      int create, int *err)
{
	struct buffer_head *bh;
	struct buffer_head dummy;

	dummy.b_state = 0;
	dummy.b_blocknr = -1000;
	*err = udf_get_block(inode, block, &dummy, create);
	if (!*err && buffer_mapped(&dummy)) {
		bh = sb_getblk(inode->i_sb, dummy.b_blocknr);
		if (buffer_new(&dummy)) {
			lock_buffer(bh);
			memset(bh->b_data, 0x00, inode->i_sb->s_blocksize);
			set_buffer_uptodate(bh);
			unlock_buffer(bh);
			mark_buffer_dirty_inode(bh, inode);
		}
		return bh;
	}

	return NULL;
}
