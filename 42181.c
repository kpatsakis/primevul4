static int hidp_session(void *arg)
{
	struct hidp_session *session = arg;
	struct sock *ctrl_sk = session->ctrl_sock->sk;
	struct sock *intr_sk = session->intr_sock->sk;
	struct sk_buff *skb;
	wait_queue_t ctrl_wait, intr_wait;

	BT_DBG("session %p", session);

	__module_get(THIS_MODULE);
	set_user_nice(current, -15);

	init_waitqueue_entry(&ctrl_wait, current);
	init_waitqueue_entry(&intr_wait, current);
	add_wait_queue(sk_sleep(ctrl_sk), &ctrl_wait);
	add_wait_queue(sk_sleep(intr_sk), &intr_wait);
	session->waiting_for_startup = 0;
	wake_up_interruptible(&session->startup_queue);
	set_current_state(TASK_INTERRUPTIBLE);
	while (!atomic_read(&session->terminate)) {
		if (ctrl_sk->sk_state != BT_CONNECTED ||
				intr_sk->sk_state != BT_CONNECTED)
			break;

		while ((skb = skb_dequeue(&intr_sk->sk_receive_queue))) {
			skb_orphan(skb);
			if (!skb_linearize(skb))
				hidp_recv_intr_frame(session, skb);
			else
				kfree_skb(skb);
		}

		hidp_process_intr_transmit(session);

		while ((skb = skb_dequeue(&ctrl_sk->sk_receive_queue))) {
			skb_orphan(skb);
			if (!skb_linearize(skb))
				hidp_recv_ctrl_frame(session, skb);
			else
				kfree_skb(skb);
		}

		hidp_process_ctrl_transmit(session);

		schedule();
		set_current_state(TASK_INTERRUPTIBLE);
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk_sleep(intr_sk), &intr_wait);
	remove_wait_queue(sk_sleep(ctrl_sk), &ctrl_wait);

	clear_bit(HIDP_WAITING_FOR_SEND_ACK, &session->flags);
	clear_bit(HIDP_WAITING_FOR_RETURN, &session->flags);
	wake_up_interruptible(&session->report_queue);

	down_write(&hidp_session_sem);

	hidp_del_timer(session);

	if (session->input) {
		input_unregister_device(session->input);
		session->input = NULL;
	}

	if (session->hid) {
		hid_destroy_device(session->hid);
		session->hid = NULL;
	}

	/* Wakeup user-space polling for socket errors */
	session->intr_sock->sk->sk_err = EUNATCH;
	session->ctrl_sock->sk->sk_err = EUNATCH;

	hidp_schedule(session);

	fput(session->intr_sock->file);

	wait_event_timeout(*(sk_sleep(ctrl_sk)),
		(ctrl_sk->sk_state == BT_CLOSED), msecs_to_jiffies(500));

	fput(session->ctrl_sock->file);

	__hidp_unlink_session(session);

	up_write(&hidp_session_sem);

	kfree(session->rd_data);
	kfree(session);
	module_put_and_exit(0);
	return 0;
}
