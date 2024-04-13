ext4_file_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(iocb->ki_filp);
	struct mutex *aio_mutex = NULL;
	struct blk_plug plug;
	int o_direct = file->f_flags & O_DIRECT;
	int overwrite = 0;
	size_t length = iov_iter_count(from);
	ssize_t ret;
	loff_t pos = iocb->ki_pos;

	/*
	 * Unaligned direct AIO must be serialized; see comment above
	 * In the case of O_APPEND, assume that we must always serialize
	 */
	if (o_direct &&
	    ext4_test_inode_flag(inode, EXT4_INODE_EXTENTS) &&
	    !is_sync_kiocb(iocb) &&
	    (file->f_flags & O_APPEND ||
	     ext4_unaligned_aio(inode, from, pos))) {
		aio_mutex = ext4_aio_mutex(inode);
		mutex_lock(aio_mutex);
		ext4_unwritten_wait(inode);
	}

	mutex_lock(&inode->i_mutex);
	if (file->f_flags & O_APPEND)
		iocb->ki_pos = pos = i_size_read(inode);

	/*
	 * If we have encountered a bitmap-format file, the size limit
	 * is smaller than s_maxbytes, which is for extent-mapped files.
	 */
	if (!(ext4_test_inode_flag(inode, EXT4_INODE_EXTENTS))) {
		struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);

		if ((pos > sbi->s_bitmap_maxbytes) ||
		    (pos == sbi->s_bitmap_maxbytes && length > 0)) {
			mutex_unlock(&inode->i_mutex);
			ret = -EFBIG;
			goto errout;
		}

		if (pos + length > sbi->s_bitmap_maxbytes)
			iov_iter_truncate(from, sbi->s_bitmap_maxbytes - pos);
	}

	if (o_direct) {
		blk_start_plug(&plug);

		iocb->private = &overwrite;

		/* check whether we do a DIO overwrite or not */
		if (ext4_should_dioread_nolock(inode) && !aio_mutex &&
		    !file->f_mapping->nrpages && pos + length <= i_size_read(inode)) {
			struct ext4_map_blocks map;
			unsigned int blkbits = inode->i_blkbits;
			int err, len;

			map.m_lblk = pos >> blkbits;
			map.m_len = (EXT4_BLOCK_ALIGN(pos + length, blkbits) >> blkbits)
				- map.m_lblk;
			len = map.m_len;

			err = ext4_map_blocks(NULL, inode, &map, 0);
			/*
			 * 'err==len' means that all of blocks has
			 * been preallocated no matter they are
			 * initialized or not.  For excluding
			 * unwritten extents, we need to check
			 * m_flags.  There are two conditions that
			 * indicate for initialized extents.  1) If we
			 * hit extent cache, EXT4_MAP_MAPPED flag is
			 * returned; 2) If we do a real lookup,
			 * non-flags are returned.  So we should check
			 * these two conditions.
			 */
			if (err == len && (map.m_flags & EXT4_MAP_MAPPED))
				overwrite = 1;
		}
	}

	ret = __generic_file_write_iter(iocb, from);
	mutex_unlock(&inode->i_mutex);

	if (ret > 0) {
		ssize_t err;

		err = generic_write_sync(file, iocb->ki_pos - ret, ret);
		if (err < 0)
			ret = err;
	}
	if (o_direct)
		blk_finish_plug(&plug);

errout:
	if (aio_mutex)
		mutex_unlock(aio_mutex);
	return ret;
}
