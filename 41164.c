static void free_mnt_ns(struct mnt_namespace *ns)
{
	proc_free_inum(ns->proc_inum);
	put_user_ns(ns->user_ns);
	kfree(ns);
}
