static __le32 ext4_dx_csum(struct inode *inode, struct ext4_dir_entry *dirent,
			   int count_offset, int count, struct dx_tail *t)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	struct ext4_inode_info *ei = EXT4_I(inode);
	__u32 csum, old_csum;
	int size;

	size = count_offset + (count * sizeof(struct dx_entry));
	old_csum = t->dt_checksum;
	t->dt_checksum = 0;
	csum = ext4_chksum(sbi, ei->i_csum_seed, (__u8 *)dirent, size);
	csum = ext4_chksum(sbi, csum, (__u8 *)t, sizeof(struct dx_tail));
	t->dt_checksum = old_csum;

	return cpu_to_le32(csum);
}
