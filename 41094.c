static struct buffer_head * ext4_dx_find_entry(struct inode *dir, const struct qstr *d_name,
		       struct ext4_dir_entry_2 **res_dir, int *err)
{
	struct super_block * sb = dir->i_sb;
	struct dx_hash_info	hinfo;
	struct dx_frame frames[2], *frame;
	struct buffer_head *bh;
	ext4_lblk_t block;
	int retval;

	if (!(frame = dx_probe(d_name, dir, &hinfo, frames, err)))
		return NULL;
	do {
		block = dx_get_block(frame->at);
		if (!(bh = ext4_bread(NULL, dir, block, 0, err))) {
			if (!(*err)) {
				*err = -EIO;
				ext4_error(dir->i_sb,
					   "Directory hole detected on inode %lu\n",
					   dir->i_ino);
			}
			goto errout;
		}

		if (!buffer_verified(bh) &&
		    !ext4_dirent_csum_verify(dir,
				(struct ext4_dir_entry *)bh->b_data)) {
			EXT4_ERROR_INODE(dir, "checksumming directory "
					 "block %lu", (unsigned long)block);
			brelse(bh);
			*err = -EIO;
			goto errout;
		}
		set_buffer_verified(bh);
		retval = search_dirblock(bh, dir, d_name,
					 block << EXT4_BLOCK_SIZE_BITS(sb),
					 res_dir);
		if (retval == 1) { 	/* Success! */
			dx_release(frames);
			return bh;
		}
		brelse(bh);
		if (retval == -1) {
			*err = ERR_BAD_DX_DIR;
			goto errout;
		}

		/* Check to see if we should continue to search */
		retval = ext4_htree_next_block(dir, hinfo.hash, frame,
					       frames, NULL);
		if (retval < 0) {
			ext4_warning(sb,
			     "error reading index page in directory #%lu",
			     dir->i_ino);
			*err = retval;
			goto errout;
		}
	} while (retval == 1);

	*err = -ENOENT;
errout:
	dxtrace(printk(KERN_DEBUG "%s not found\n", d_name->name));
	dx_release (frames);
	return NULL;
}
