void nfs_commitdata_release(struct nfs_commit_data *data)
{
	put_nfs_open_context(data->context);
	nfs_commit_free(data);
}
