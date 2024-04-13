static unsigned f2fs_max_namelen(struct inode *inode)
{
	return S_ISLNK(inode->i_mode) ?
			inode->i_sb->s_blocksize : F2FS_NAME_LEN;
}
