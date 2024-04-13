mm_answer_sessid(int sock, Buffer *m)
{
	int i;

	debug3("%s entering", __func__);

	if (buffer_len(m) != 16)
		fatal("%s: bad ssh1 session id", __func__);
	for (i = 0; i < 16; i++)
		session_id[i] = buffer_get_char(m);

	/* Turn on permissions for getpwnam */
	monitor_permit(mon_dispatch, MONITOR_REQ_PWNAM, 1);

	return (0);
}
