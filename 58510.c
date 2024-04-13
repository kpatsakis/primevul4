int nfsd_pool_stats_release(struct inode *inode, struct file *file)
{
	int ret = seq_release(inode, file);
	struct net *net = inode->i_sb->s_fs_info;

	mutex_lock(&nfsd_mutex);
	/* this function really, really should have been called svc_put() */
	nfsd_destroy(net);
	mutex_unlock(&nfsd_mutex);
	return ret;
}
