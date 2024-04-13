static struct dentry *udf_fh_to_parent(struct super_block *sb,
				       struct fid *fid, int fh_len, int fh_type)
{
	if (fh_len != 5 || fh_type != FILEID_UDF_WITH_PARENT)
		return NULL;

	return udf_nfs_get_inode(sb, fid->udf.parent_block,
				 fid->udf.parent_partref,
				 fid->udf.parent_generation);
}
