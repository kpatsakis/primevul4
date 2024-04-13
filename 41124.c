static inline int search_dirblock(struct buffer_head *bh,
				  struct inode *dir,
				  const struct qstr *d_name,
				  unsigned int offset,
				  struct ext4_dir_entry_2 **res_dir)
{
	return search_dir(bh, bh->b_data, dir->i_sb->s_blocksize, dir,
			  d_name, offset, res_dir);
}
