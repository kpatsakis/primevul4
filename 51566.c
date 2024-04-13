static void audit_log_set_loginuid(kuid_t koldloginuid, kuid_t kloginuid,
				   unsigned int oldsessionid, unsigned int sessionid,
				   int rc)
{
	struct audit_buffer *ab;
	uid_t uid, oldloginuid, loginuid;

	if (!audit_enabled)
		return;

	uid = from_kuid(&init_user_ns, task_uid(current));
	oldloginuid = from_kuid(&init_user_ns, koldloginuid);
	loginuid = from_kuid(&init_user_ns, kloginuid),

	ab = audit_log_start(NULL, GFP_KERNEL, AUDIT_LOGIN);
	if (!ab)
		return;
	audit_log_format(ab, "pid=%d uid=%u", task_pid_nr(current), uid);
	audit_log_task_context(ab);
	audit_log_format(ab, " old-auid=%u auid=%u old-ses=%u ses=%u res=%d",
			 oldloginuid, loginuid, oldsessionid, sessionid, !rc);
	audit_log_end(ab);
}
