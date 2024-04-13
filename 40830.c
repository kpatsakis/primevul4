static int kill_as_cred_perm(const struct cred *cred,
			     struct task_struct *target)
{
	const struct cred *pcred = __task_cred(target);
	if (!uid_eq(cred->euid, pcred->suid) && !uid_eq(cred->euid, pcred->uid) &&
	    !uid_eq(cred->uid,  pcred->suid) && !uid_eq(cred->uid,  pcred->uid))
		return 0;
	return 1;
}
