static int f2fs_set_context(struct inode *inode, const void *ctx, size_t len,
							void *fs_data)
{
	return f2fs_setxattr(inode, F2FS_XATTR_INDEX_ENCRYPTION,
				F2FS_XATTR_NAME_ENCRYPTION_CONTEXT,
				ctx, len, fs_data, XATTR_CREATE);
}
