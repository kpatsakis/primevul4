static int kill_ok_by_cred(struct task_struct *t)
{
	const struct cred *cred = current_cred();
	const struct cred *tcred = __task_cred(t);

	if (uid_eq(cred->euid, tcred->suid) ||
	    uid_eq(cred->euid, tcred->uid)  ||
	    uid_eq(cred->uid,  tcred->suid) ||
	    uid_eq(cred->uid,  tcred->uid))
		return 1;

	if (ns_capable(tcred->user_ns, CAP_KILL))
		return 1;

	return 0;
}
