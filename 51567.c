static void audit_log_task(struct audit_buffer *ab)
{
	kuid_t auid, uid;
	kgid_t gid;
	unsigned int sessionid;
	char comm[sizeof(current->comm)];

	auid = audit_get_loginuid(current);
	sessionid = audit_get_sessionid(current);
	current_uid_gid(&uid, &gid);

	audit_log_format(ab, "auid=%u uid=%u gid=%u ses=%u",
			 from_kuid(&init_user_ns, auid),
			 from_kuid(&init_user_ns, uid),
			 from_kgid(&init_user_ns, gid),
			 sessionid);
	audit_log_task_context(ab);
	audit_log_format(ab, " pid=%d comm=", task_pid_nr(current));
	audit_log_untrustedstring(ab, get_task_comm(comm, current));
	audit_log_d_path_exe(ab, current->mm);
}
