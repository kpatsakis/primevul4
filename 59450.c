vc4_job_handle_completed(struct vc4_dev *vc4)
{
	unsigned long irqflags;
	struct vc4_seqno_cb *cb, *cb_temp;

	spin_lock_irqsave(&vc4->job_lock, irqflags);
	while (!list_empty(&vc4->job_done_list)) {
		struct vc4_exec_info *exec =
			list_first_entry(&vc4->job_done_list,
					 struct vc4_exec_info, head);
		list_del(&exec->head);

		spin_unlock_irqrestore(&vc4->job_lock, irqflags);
		vc4_complete_exec(vc4->dev, exec);
		spin_lock_irqsave(&vc4->job_lock, irqflags);
	}

	list_for_each_entry_safe(cb, cb_temp, &vc4->seqno_cb_list, work.entry) {
		if (cb->seqno <= vc4->finished_seqno) {
			list_del_init(&cb->work.entry);
			schedule_work(&cb->work);
		}
	}

	spin_unlock_irqrestore(&vc4->job_lock, irqflags);
}
