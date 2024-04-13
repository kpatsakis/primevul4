static void wake_futex(struct futex_q *q)
{
	struct task_struct *p = q->task;

	/*
	 * We set q->lock_ptr = NULL _before_ we wake up the task. If
	 * a non-futex wake up happens on another CPU then the task
	 * might exit and p would dereference a non-existing task
	 * struct. Prevent this by holding a reference on p across the
	 * wake up.
	 */
	get_task_struct(p);

	__unqueue_futex(q);
	/*
	 * The waiting task can free the futex_q as soon as
	 * q->lock_ptr = NULL is written, without taking any locks. A
	 * memory barrier is required here to prevent the following
	 * store to lock_ptr from getting ahead of the plist_del.
	 */
	smp_wmb();
	q->lock_ptr = NULL;

	wake_up_state(p, TASK_NORMAL);
	put_task_struct(p);
}
