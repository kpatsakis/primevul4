static int ap_poll_thread(void *data)
{
	DECLARE_WAITQUEUE(wait, current);
	unsigned long flags;
	int requests;
	struct ap_device *ap_dev;

	set_user_nice(current, MAX_NICE);
	while (1) {
		if (ap_suspend_flag)
			return 0;
		if (need_resched()) {
			schedule();
			continue;
		}
		add_wait_queue(&ap_poll_wait, &wait);
		set_current_state(TASK_INTERRUPTIBLE);
		if (kthread_should_stop())
			break;
		requests = atomic_read(&ap_poll_requests);
		if (requests <= 0)
			schedule();
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&ap_poll_wait, &wait);

		flags = 0;
		spin_lock_bh(&ap_device_list_lock);
		list_for_each_entry(ap_dev, &ap_device_list, list) {
			spin_lock(&ap_dev->lock);
			__ap_poll_device(ap_dev, &flags);
			spin_unlock(&ap_dev->lock);
		}
		spin_unlock_bh(&ap_device_list_lock);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&ap_poll_wait, &wait);
	return 0;
}
