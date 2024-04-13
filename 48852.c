static long gfs2_fallocate(struct file *file, int mode, loff_t offset,
			   loff_t len)
{
	struct inode *inode = file_inode(file);
	struct gfs2_sbd *sdp = GFS2_SB(inode);
	struct gfs2_inode *ip = GFS2_I(inode);
	struct gfs2_alloc_parms ap = { .aflags = 0, };
	unsigned int data_blocks = 0, ind_blocks = 0, rblocks;
	loff_t bytes, max_bytes;
	int error;
	const loff_t pos = offset;
	const loff_t count = len;
	loff_t bsize_mask = ~((loff_t)sdp->sd_sb.sb_bsize - 1);
	loff_t next = (offset + len - 1) >> sdp->sd_sb.sb_bsize_shift;
	loff_t max_chunk_size = UINT_MAX & bsize_mask;
	struct gfs2_holder gh;

	next = (next + 1) << sdp->sd_sb.sb_bsize_shift;

	/* We only support the FALLOC_FL_KEEP_SIZE mode */
	if (mode & ~FALLOC_FL_KEEP_SIZE)
		return -EOPNOTSUPP;

	offset &= bsize_mask;

	len = next - offset;
	bytes = sdp->sd_max_rg_data * sdp->sd_sb.sb_bsize / 2;
	if (!bytes)
		bytes = UINT_MAX;
	bytes &= bsize_mask;
	if (bytes == 0)
		bytes = sdp->sd_sb.sb_bsize;

	error = gfs2_rs_alloc(ip);
	if (error)
		return error;

	mutex_lock(&inode->i_mutex);

	gfs2_holder_init(ip->i_gl, LM_ST_EXCLUSIVE, 0, &gh);
	error = gfs2_glock_nq(&gh);
	if (unlikely(error))
		goto out_uninit;

	gfs2_size_hint(file, offset, len);

	while (len > 0) {
		if (len < bytes)
			bytes = len;
		if (!gfs2_write_alloc_required(ip, offset, bytes)) {
			len -= bytes;
			offset += bytes;
			continue;
		}
		error = gfs2_quota_lock_check(ip);
		if (error)
			goto out_unlock;

retry:
		gfs2_write_calc_reserv(ip, bytes, &data_blocks, &ind_blocks);

		ap.target = data_blocks + ind_blocks;
		error = gfs2_inplace_reserve(ip, &ap);
		if (error) {
			if (error == -ENOSPC && bytes > sdp->sd_sb.sb_bsize) {
				bytes >>= 1;
				bytes &= bsize_mask;
				if (bytes == 0)
					bytes = sdp->sd_sb.sb_bsize;
				goto retry;
			}
			goto out_qunlock;
		}
		max_bytes = bytes;
		calc_max_reserv(ip, (len > max_chunk_size)? max_chunk_size: len,
				&max_bytes, &data_blocks, &ind_blocks);

		rblocks = RES_DINODE + ind_blocks + RES_STATFS + RES_QUOTA +
			  RES_RG_HDR + gfs2_rg_blocks(ip, data_blocks + ind_blocks);
		if (gfs2_is_jdata(ip))
			rblocks += data_blocks ? data_blocks : 1;

		error = gfs2_trans_begin(sdp, rblocks,
					 PAGE_CACHE_SIZE/sdp->sd_sb.sb_bsize);
		if (error)
			goto out_trans_fail;

		error = fallocate_chunk(inode, offset, max_bytes, mode);
		gfs2_trans_end(sdp);

		if (error)
			goto out_trans_fail;

		len -= max_bytes;
		offset += max_bytes;
		gfs2_inplace_release(ip);
		gfs2_quota_unlock(ip);
	}

	if (error == 0)
		error = generic_write_sync(file, pos, count);
	goto out_unlock;

out_trans_fail:
	gfs2_inplace_release(ip);
out_qunlock:
	gfs2_quota_unlock(ip);
out_unlock:
	gfs2_glock_dq(&gh);
out_uninit:
	gfs2_holder_uninit(&gh);
	mutex_unlock(&inode->i_mutex);
	return error;
}
