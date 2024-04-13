static void cred_to_ucred(struct pid *pid, const struct cred *cred,
			  struct ucred *ucred)
{
	ucred->pid = pid_vnr(pid);
	ucred->uid = ucred->gid = -1;
	if (cred) {
		struct user_namespace *current_ns = current_user_ns();

		ucred->uid = from_kuid_munged(current_ns, cred->euid);
		ucred->gid = from_kgid_munged(current_ns, cred->egid);
	}
}
