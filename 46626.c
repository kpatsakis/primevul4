void md_cluster_stop(struct mddev *mddev)
{
	if (!md_cluster_ops)
		return;
	md_cluster_ops->leave(mddev);
	module_put(md_cluster_mod);
}
