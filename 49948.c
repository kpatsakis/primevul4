static void ap_reset(struct ap_device *ap_dev)
{
	int rc;

	ap_dev->reset = AP_RESET_IGNORE;
	atomic_sub(ap_dev->queue_count, &ap_poll_requests);
	ap_dev->queue_count = 0;
	list_splice_init(&ap_dev->pendingq, &ap_dev->requestq);
	ap_dev->requestq_count += ap_dev->pendingq_count;
	ap_dev->pendingq_count = 0;
	rc = ap_init_queue(ap_dev->qid);
	if (rc == -ENODEV)
		ap_dev->unregistered = 1;
	else
		__ap_schedule_poll_timer();
}
