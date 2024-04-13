vc4_queue_submit(struct drm_device *dev, struct vc4_exec_info *exec)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	uint64_t seqno;
	unsigned long irqflags;

	spin_lock_irqsave(&vc4->job_lock, irqflags);

	seqno = ++vc4->emit_seqno;
	exec->seqno = seqno;
	vc4_update_bo_seqnos(exec, seqno);

	list_add_tail(&exec->head, &vc4->bin_job_list);

	/* If no job was executing, kick ours off.  Otherwise, it'll
	 * get started when the previous job's flush done interrupt
	 * occurs.
	 */
	if (vc4_first_bin_job(vc4) == exec) {
		vc4_submit_next_bin_job(dev);
		vc4_queue_hangcheck(dev);
	}

	spin_unlock_irqrestore(&vc4->job_lock, irqflags);
}
