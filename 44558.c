void nfs_pageio_init_write(struct nfs_pageio_descriptor *pgio,
			       struct inode *inode, int ioflags,
			       const struct nfs_pgio_completion_ops *compl_ops)
{
	nfs_pageio_init(pgio, inode, &nfs_pageio_write_ops, compl_ops,
				NFS_SERVER(inode)->wsize, ioflags);
}
