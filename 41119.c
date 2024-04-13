static struct ext4_dir_entry_tail *get_dirent_tail(struct inode *inode,
						   struct ext4_dir_entry *de)
{
	struct ext4_dir_entry_tail *t;

#ifdef PARANOID
	struct ext4_dir_entry *d, *top;

	d = de;
	top = (struct ext4_dir_entry *)(((void *)de) +
		(EXT4_BLOCK_SIZE(inode->i_sb) -
		sizeof(struct ext4_dir_entry_tail)));
	while (d < top && d->rec_len)
		d = (struct ext4_dir_entry *)(((void *)d) +
		    le16_to_cpu(d->rec_len));

	if (d != top)
		return NULL;

	t = (struct ext4_dir_entry_tail *)d;
#else
	t = EXT4_DIRENT_TAIL(de, EXT4_BLOCK_SIZE(inode->i_sb));
#endif

	if (t->det_reserved_zero1 ||
	    le16_to_cpu(t->det_rec_len) != sizeof(struct ext4_dir_entry_tail) ||
	    t->det_reserved_zero2 ||
	    t->det_reserved_ft != EXT4_FT_DIR_CSUM)
		return NULL;

	return t;
}
