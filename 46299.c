static void ss_wakeup(struct list_head *h, int kill)
{
	struct msg_sender *mss, *t;

	list_for_each_entry_safe(mss, t, h, list) {
		if (kill)
			mss->list.next = NULL;
		wake_up_process(mss->tsk);
	}
}
