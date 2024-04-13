struct buffer_head *udf_bread(struct inode *inode, int block,
			      int create, int *err)
{
	struct buffer_head *bh = NULL;

	bh = udf_getblk(inode, block, create, err);
	if (!bh)
		return NULL;

	if (buffer_uptodate(bh))
		return bh;

	ll_rw_block(READ, 1, &bh);

	wait_on_buffer(bh);
	if (buffer_uptodate(bh))
		return bh;

	brelse(bh);
	*err = -EIO;
	return NULL;
}
