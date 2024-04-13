static void free_mnt_ns(struct mnt_namespace *ns)
{
	ns_free_inum(&ns->ns);
	put_user_ns(ns->user_ns);
	kfree(ns);
}
