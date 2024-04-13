void __audit_bprm(struct linux_binprm *bprm)
{
	struct audit_context *context = current->audit_context;

	context->type = AUDIT_EXECVE;
	context->execve.argc = bprm->argc;
}
