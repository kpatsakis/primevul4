static int vhost_worker(void *data)
{
	struct vhost_dev *dev = data;
	struct vhost_work *work = NULL;
	unsigned uninitialized_var(seq);
	mm_segment_t oldfs = get_fs();

	set_fs(USER_DS);
	use_mm(dev->mm);

	for (;;) {
		/* mb paired w/ kthread_stop */
		set_current_state(TASK_INTERRUPTIBLE);

		spin_lock_irq(&dev->work_lock);
		if (work) {
			work->done_seq = seq;
			if (work->flushing)
				wake_up_all(&work->done);
		}

		if (kthread_should_stop()) {
			spin_unlock_irq(&dev->work_lock);
			__set_current_state(TASK_RUNNING);
			break;
		}
		if (!list_empty(&dev->work_list)) {
			work = list_first_entry(&dev->work_list,
						struct vhost_work, node);
			list_del_init(&work->node);
			seq = work->queue_seq;
		} else
			work = NULL;
		spin_unlock_irq(&dev->work_lock);

		if (work) {
			__set_current_state(TASK_RUNNING);
			work->fn(work);
			if (need_resched())
				schedule();
		} else
			schedule();

	}
	unuse_mm(dev->mm);
	set_fs(oldfs);
	return 0;
}
