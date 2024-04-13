void cred_to_ucred(struct pid *pid, const struct cred *cred,
		   struct ucred *ucred)
{
	ucred->pid = pid_vnr(pid);
	ucred->uid = ucred->gid = -1;
	if (cred) {
		struct user_namespace *current_ns = current_user_ns();

		ucred->uid = user_ns_map_uid(current_ns, cred, cred->euid);
		ucred->gid = user_ns_map_gid(current_ns, cred, cred->egid);
	}
}
