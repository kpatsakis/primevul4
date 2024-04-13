void ext4_insert_dentry(struct inode *inode,
			struct ext4_dir_entry_2 *de,
			int buf_size,
			const char *name, int namelen)
{

	int nlen, rlen;

	nlen = EXT4_DIR_REC_LEN(de->name_len);
	rlen = ext4_rec_len_from_disk(de->rec_len, buf_size);
	if (de->inode) {
		struct ext4_dir_entry_2 *de1 =
				(struct ext4_dir_entry_2 *)((char *)de + nlen);
		de1->rec_len = ext4_rec_len_to_disk(rlen - nlen, buf_size);
		de->rec_len = ext4_rec_len_to_disk(nlen, buf_size);
		de = de1;
	}
	de->file_type = EXT4_FT_UNKNOWN;
	de->inode = cpu_to_le32(inode->i_ino);
	ext4_set_de_type(inode->i_sb, de, inode->i_mode);
	de->name_len = namelen;
	memcpy(de->name, name, namelen);
}
