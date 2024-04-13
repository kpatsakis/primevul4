static void vc4_seqno_cb_work(struct work_struct *work)
{
	struct vc4_seqno_cb *cb = container_of(work, struct vc4_seqno_cb, work);

	cb->func(cb);
}
