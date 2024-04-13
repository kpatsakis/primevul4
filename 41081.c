static int empty_dir(struct inode *inode)
{
	unsigned int offset;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de, *de1;
	struct super_block *sb;
	int err = 0;

	if (ext4_has_inline_data(inode)) {
		int has_inline_data = 1;

		err = empty_inline_dir(inode, &has_inline_data);
		if (has_inline_data)
			return err;
	}

	sb = inode->i_sb;
	if (inode->i_size < EXT4_DIR_REC_LEN(1) + EXT4_DIR_REC_LEN(2) ||
	    !(bh = ext4_bread(NULL, inode, 0, 0, &err))) {
		if (err)
			EXT4_ERROR_INODE(inode,
				"error %d reading directory lblock 0", err);
		else
			ext4_warning(inode->i_sb,
				     "bad directory (dir #%lu) - no data block",
				     inode->i_ino);
		return 1;
	}
	if (!buffer_verified(bh) &&
	    !ext4_dirent_csum_verify(inode,
			(struct ext4_dir_entry *)bh->b_data)) {
		EXT4_ERROR_INODE(inode, "checksum error reading directory "
				 "lblock 0");
		return -EIO;
	}
	set_buffer_verified(bh);
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	de1 = ext4_next_entry(de, sb->s_blocksize);
	if (le32_to_cpu(de->inode) != inode->i_ino ||
			!le32_to_cpu(de1->inode) ||
			strcmp(".", de->name) ||
			strcmp("..", de1->name)) {
		ext4_warning(inode->i_sb,
			     "bad directory (dir #%lu) - no `.' or `..'",
			     inode->i_ino);
		brelse(bh);
		return 1;
	}
	offset = ext4_rec_len_from_disk(de->rec_len, sb->s_blocksize) +
		 ext4_rec_len_from_disk(de1->rec_len, sb->s_blocksize);
	de = ext4_next_entry(de1, sb->s_blocksize);
	while (offset < inode->i_size) {
		if (!bh ||
		    (void *) de >= (void *) (bh->b_data+sb->s_blocksize)) {
			unsigned int lblock;
			err = 0;
			brelse(bh);
			lblock = offset >> EXT4_BLOCK_SIZE_BITS(sb);
			bh = ext4_bread(NULL, inode, lblock, 0, &err);
			if (!bh) {
				if (err)
					EXT4_ERROR_INODE(inode,
						"error %d reading directory "
						"lblock %u", err, lblock);
				else
					ext4_warning(inode->i_sb,
						"bad directory (dir #%lu) - no data block",
						inode->i_ino);

				offset += sb->s_blocksize;
				continue;
			}
			if (!buffer_verified(bh) &&
			    !ext4_dirent_csum_verify(inode,
					(struct ext4_dir_entry *)bh->b_data)) {
				EXT4_ERROR_INODE(inode, "checksum error "
						 "reading directory lblock 0");
				return -EIO;
			}
			set_buffer_verified(bh);
			de = (struct ext4_dir_entry_2 *) bh->b_data;
		}
		if (ext4_check_dir_entry(inode, NULL, de, bh,
					 bh->b_data, bh->b_size, offset)) {
			de = (struct ext4_dir_entry_2 *)(bh->b_data +
							 sb->s_blocksize);
			offset = (offset | (sb->s_blocksize - 1)) + 1;
			continue;
		}
		if (le32_to_cpu(de->inode)) {
			brelse(bh);
			return 0;
		}
		offset += ext4_rec_len_from_disk(de->rec_len, sb->s_blocksize);
		de = ext4_next_entry(de, sb->s_blocksize);
	}
	brelse(bh);
	return 1;
}
