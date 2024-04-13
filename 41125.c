static void warn_no_space_for_csum(struct inode *inode)
{
	ext4_warning(inode->i_sb, "no space in directory inode %lu leaf for "
		     "checksum.  Please run e2fsck -D.", inode->i_ino);
}
