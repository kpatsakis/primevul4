static void apparmor_cred_transfer(struct cred *new, const struct cred *old)
{
	const struct aa_task_cxt *old_cxt = old->security;
	struct aa_task_cxt *new_cxt = new->security;

	aa_dup_task_context(new_cxt, old_cxt);
}
