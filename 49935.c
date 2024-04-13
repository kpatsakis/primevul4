static void ap_poll_thread_stop(void)
{
	mutex_lock(&ap_poll_thread_mutex);
	if (ap_poll_kthread) {
		kthread_stop(ap_poll_kthread);
		ap_poll_kthread = NULL;
	}
	mutex_unlock(&ap_poll_thread_mutex);
}
