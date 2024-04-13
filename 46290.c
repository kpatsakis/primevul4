static void expunge_all(struct msg_queue *msq, int res)
{
	struct msg_receiver *msr, *t;

	list_for_each_entry_safe(msr, t, &msq->q_receivers, r_list) {
		msr->r_msg = NULL; /* initialize expunge ordering */
		wake_up_process(msr->r_tsk);
		/*
		 * Ensure that the wakeup is visible before setting r_msg as
		 * the receiving end depends on it: either spinning on a nil,
		 * or dealing with -EAGAIN cases. See lockless receive part 1
		 * and 2 in do_msgrcv().
		 */
		smp_wmb(); /* barrier (B) */
		msr->r_msg = ERR_PTR(res);
	}
}
