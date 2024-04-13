static void hidp_idle_timeout(unsigned long arg)
{
	struct hidp_session *session = (struct hidp_session *) arg;

	atomic_inc(&session->terminate);
	wake_up_process(session->task);
}
