int register_md_cluster_operations(struct md_cluster_operations *ops, struct module *module)
{
	if (md_cluster_ops != NULL)
		return -EALREADY;
	spin_lock(&pers_lock);
	md_cluster_ops = ops;
	md_cluster_mod = module;
	spin_unlock(&pers_lock);
	return 0;
}
