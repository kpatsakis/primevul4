static void apparmor_cred_transfer(struct cred *new, const struct cred *old)
{
	const struct aa_task_cxt *old_cxt = cred_cxt(old);
	struct aa_task_cxt *new_cxt = cred_cxt(new);

	aa_dup_task_context(new_cxt, old_cxt);
}
