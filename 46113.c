static bool ovl_need_xattr_filter(struct dentry *dentry,
				  enum ovl_path_type type)
{
	if ((type & (__OVL_PATH_PURE | __OVL_PATH_UPPER)) == __OVL_PATH_UPPER)
		return S_ISDIR(dentry->d_inode->i_mode);
	else
		return false;
}
