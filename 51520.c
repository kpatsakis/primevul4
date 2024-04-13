static int __init set_init_cxt(void)
{
	struct cred *cred = (struct cred *)current->real_cred;
	struct aa_task_cxt *cxt;

	cxt = aa_alloc_task_context(GFP_KERNEL);
	if (!cxt)
		return -ENOMEM;

	cxt->profile = aa_get_profile(root_ns->unconfined);
	cred_cxt(cred) = cxt;

	return 0;
}
