struct md_thread *md_register_thread(void (*run) (struct md_thread *),
		struct mddev *mddev, const char *name)
{
	struct md_thread *thread;

	thread = kzalloc(sizeof(struct md_thread), GFP_KERNEL);
	if (!thread)
		return NULL;

	init_waitqueue_head(&thread->wqueue);

	thread->run = run;
	thread->mddev = mddev;
	thread->timeout = MAX_SCHEDULE_TIMEOUT;
	thread->tsk = kthread_run(md_thread, thread,
				  "%s_%s",
				  mdname(thread->mddev),
				  name);
	if (IS_ERR(thread->tsk)) {
		kfree(thread);
		return NULL;
	}
	return thread;
}
