static void __udf_clear_extent_cache(struct inode *inode)
{
	struct udf_inode_info *iinfo = UDF_I(inode);

	if (iinfo->cached_extent.lstart != -1) {
		brelse(iinfo->cached_extent.epos.bh);
		iinfo->cached_extent.lstart = -1;
	}
}
