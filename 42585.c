static void apparmor_cred_free(struct cred *cred)
{
	aa_free_task_context(cred->security);
	cred->security = NULL;
}
