void nfs_pageio_reset_write_mds(struct nfs_pageio_descriptor *pgio)
{
	pgio->pg_ops = &nfs_pageio_write_ops;
	pgio->pg_bsize = NFS_SERVER(pgio->pg_inode)->wsize;
}
