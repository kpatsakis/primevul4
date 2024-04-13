static int f2fs_get_context(struct inode *inode, void *ctx, size_t len)
{
	return f2fs_getxattr(inode, F2FS_XATTR_INDEX_ENCRYPTION,
				F2FS_XATTR_NAME_ENCRYPTION_CONTEXT,
				ctx, len, NULL);
}
