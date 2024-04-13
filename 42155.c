static void hidp_del_timer(struct hidp_session *session)
{
	if (session->idle_to > 0)
		del_timer(&session->timer);
}
