static struct ext4_dir_entry_2* dx_pack_dirents(char *base, unsigned blocksize)
{
	struct ext4_dir_entry_2 *next, *to, *prev, *de = (struct ext4_dir_entry_2 *) base;
	unsigned rec_len = 0;

	prev = to = de;
	while ((char*)de < base + blocksize) {
		next = ext4_next_entry(de, blocksize);
		if (de->inode && de->name_len) {
			rec_len = EXT4_DIR_REC_LEN(de->name_len);
			if (de > to)
				memmove(to, de, rec_len);
			to->rec_len = ext4_rec_len_to_disk(rec_len, blocksize);
			prev = to;
			to = (struct ext4_dir_entry_2 *) (((char *) to) + rec_len);
		}
		de = next;
	}
	return prev;
}
