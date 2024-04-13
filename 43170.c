static void udf_clear_extent_cache(struct inode *inode)
{
	struct udf_inode_info *iinfo = UDF_I(inode);

	spin_lock(&iinfo->i_extent_cache_lock);
	__udf_clear_extent_cache(inode);
	spin_unlock(&iinfo->i_extent_cache_lock);
}
