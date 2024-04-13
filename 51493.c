static void apparmor_cred_free(struct cred *cred)
{
	aa_free_task_context(cred_cxt(cred));
	cred_cxt(cred) = NULL;
}
