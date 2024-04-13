static void hidp_set_timer(struct hidp_session *session)
{
	if (session->idle_to > 0)
		mod_timer(&session->timer, jiffies + HZ * session->idle_to);
}
