static struct dentry *isofs_fh_to_parent(struct super_block *sb,
		struct fid *fid, int fh_len, int fh_type)
{
	struct isofs_fid *ifid = (struct isofs_fid *)fid;

	if (fh_type != 2)
		return NULL;

	return isofs_export_iget(sb,
			fh_len > 2 ? ifid->parent_block : 0,
			ifid->parent_offset,
			fh_len > 4 ? ifid->parent_generation : 0);
}
