int hidp_get_conninfo(struct hidp_conninfo *ci)
{
	struct hidp_session *session;
	int err = 0;

	down_read(&hidp_session_sem);

	session = __hidp_get_session(&ci->bdaddr);
	if (session)
		__hidp_copy_session(session, ci);
	else
		err = -ENOENT;

	up_read(&hidp_session_sem);
	return err;
}
