static void gfs2_size_hint(struct file *filep, loff_t offset, size_t size)
{
	struct inode *inode = file_inode(filep);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct gfs2_inode *ip = GFS2_I(inode);
	size_t blks = (size + sdp->sd_sb.sb_bsize - 1) >> sdp->sd_sb.sb_bsize_shift;
	int hint = min_t(size_t, INT_MAX, blks);

	atomic_set(&ip->i_res->rs_sizehint, hint);
}
