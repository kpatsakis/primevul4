void md_new_event(struct mddev *mddev)
{
	atomic_inc(&md_event_count);
	wake_up(&md_event_waiters);
}
