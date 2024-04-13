static inline void pipelined_send(struct wake_q_head *wake_q,
				  struct mqueue_inode_info *info,
				  struct msg_msg *message,
				  struct ext_wait_queue *receiver)
{
	receiver->msg = message;
	list_del(&receiver->list);
	wake_q_add(wake_q, receiver->task);
	/*
	 * Rely on the implicit cmpxchg barrier from wake_q_add such
	 * that we can ensure that updating receiver->state is the last
	 * write operation: As once set, the receiver can continue,
	 * and if we don't have the reference count from the wake_q,
	 * yet, at that point we can later have a use-after-free
	 * condition and bogus wakeup.
	 */
	receiver->state = STATE_READY;
}
