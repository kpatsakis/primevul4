static inline int pipelined_send(struct msg_queue *msq, struct msg_msg *msg)
{
	struct msg_receiver *msr, *t;

	list_for_each_entry_safe(msr, t, &msq->q_receivers, r_list) {
		if (testmsg(msg, msr->r_msgtype, msr->r_mode) &&
		    !security_msg_queue_msgrcv(msq, msg, msr->r_tsk,
					       msr->r_msgtype, msr->r_mode)) {

			list_del(&msr->r_list);
			if (msr->r_maxsize < msg->m_ts) {
				/* initialize pipelined send ordering */
				msr->r_msg = NULL;
				wake_up_process(msr->r_tsk);
				/* barrier (B) see barrier comment below */
				smp_wmb();
				msr->r_msg = ERR_PTR(-E2BIG);
			} else {
				msr->r_msg = NULL;
				msq->q_lrpid = task_pid_vnr(msr->r_tsk);
				msq->q_rtime = get_seconds();
				wake_up_process(msr->r_tsk);
				/*
				 * Ensure that the wakeup is visible before
				 * setting r_msg, as the receiving can otherwise
				 * exit - once r_msg is set, the receiver can
				 * continue. See lockless receive part 1 and 2
				 * in do_msgrcv(). Barrier (B).
				 */
				smp_wmb();
				msr->r_msg = msg;

				return 1;
			}
		}
	}

	return 0;
}
