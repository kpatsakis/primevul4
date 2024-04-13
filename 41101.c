int ext4_htree_fill_tree(struct file *dir_file, __u32 start_hash,
			 __u32 start_minor_hash, __u32 *next_hash)
{
	struct dx_hash_info hinfo;
	struct ext4_dir_entry_2 *de;
	struct dx_frame frames[2], *frame;
	struct inode *dir;
	ext4_lblk_t block;
	int count = 0;
	int ret, err;
	__u32 hashval;

	dxtrace(printk(KERN_DEBUG "In htree_fill_tree, start hash: %x:%x\n",
		       start_hash, start_minor_hash));
	dir = dir_file->f_path.dentry->d_inode;
	if (!(ext4_test_inode_flag(dir, EXT4_INODE_INDEX))) {
		hinfo.hash_version = EXT4_SB(dir->i_sb)->s_def_hash_version;
		if (hinfo.hash_version <= DX_HASH_TEA)
			hinfo.hash_version +=
				EXT4_SB(dir->i_sb)->s_hash_unsigned;
		hinfo.seed = EXT4_SB(dir->i_sb)->s_hash_seed;
		count = htree_dirblock_to_tree(dir_file, dir, 0, &hinfo,
					       start_hash, start_minor_hash);
		*next_hash = ~0;
		return count;
	}
	hinfo.hash = start_hash;
	hinfo.minor_hash = 0;
	frame = dx_probe(NULL, dir, &hinfo, frames, &err);
	if (!frame)
		return err;

	/* Add '.' and '..' from the htree header */
	if (!start_hash && !start_minor_hash) {
		de = (struct ext4_dir_entry_2 *) frames[0].bh->b_data;
		if ((err = ext4_htree_store_dirent(dir_file, 0, 0, de)) != 0)
			goto errout;
		count++;
	}
	if (start_hash < 2 || (start_hash ==2 && start_minor_hash==0)) {
		de = (struct ext4_dir_entry_2 *) frames[0].bh->b_data;
		de = ext4_next_entry(de, dir->i_sb->s_blocksize);
		if ((err = ext4_htree_store_dirent(dir_file, 2, 0, de)) != 0)
			goto errout;
		count++;
	}

	while (1) {
		block = dx_get_block(frame->at);
		ret = htree_dirblock_to_tree(dir_file, dir, block, &hinfo,
					     start_hash, start_minor_hash);
		if (ret < 0) {
			err = ret;
			goto errout;
		}
		count += ret;
		hashval = ~0;
		ret = ext4_htree_next_block(dir, HASH_NB_ALWAYS,
					    frame, frames, &hashval);
		*next_hash = hashval;
		if (ret < 0) {
			err = ret;
			goto errout;
		}
		/*
		 * Stop if:  (a) there are no more entries, or
		 * (b) we have inserted at least one entry and the
		 * next hash value is not a continuation
		 */
		if ((ret == 0) ||
		    (count && ((hashval & 1) == 0)))
			break;
	}
	dx_release(frames);
	dxtrace(printk(KERN_DEBUG "Fill tree: returned %d entries, "
		       "next hash: %x\n", count, *next_hash));
	return count;
errout:
	dx_release(frames);
	return (err);
}
