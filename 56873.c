static inline void pipelined_receive(struct wake_q_head *wake_q,
				     struct mqueue_inode_info *info)
{
	struct ext_wait_queue *sender = wq_get_first_waiter(info, SEND);

	if (!sender) {
		/* for poll */
		wake_up_interruptible(&info->wait_q);
		return;
	}
	if (msg_insert(sender->msg, info))
		return;

	list_del(&sender->list);
	wake_q_add(wake_q, sender->task);
	sender->state = STATE_READY;
}
