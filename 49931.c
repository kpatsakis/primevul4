static int ap_poll_read(struct ap_device *ap_dev, unsigned long *flags)
{
	struct ap_queue_status status;
	struct ap_message *ap_msg;

	if (ap_dev->queue_count <= 0)
		return 0;
	status = __ap_recv(ap_dev->qid, &ap_dev->reply->psmid,
			   ap_dev->reply->message, ap_dev->reply->length);
	switch (status.response_code) {
	case AP_RESPONSE_NORMAL:
		atomic_dec(&ap_poll_requests);
		ap_decrease_queue_count(ap_dev);
		list_for_each_entry(ap_msg, &ap_dev->pendingq, list) {
			if (ap_msg->psmid != ap_dev->reply->psmid)
				continue;
			list_del_init(&ap_msg->list);
			ap_dev->pendingq_count--;
			ap_msg->receive(ap_dev, ap_msg, ap_dev->reply);
			break;
		}
		if (ap_dev->queue_count > 0)
			*flags |= 1;
		break;
	case AP_RESPONSE_NO_PENDING_REPLY:
		if (status.queue_empty) {
			/* The card shouldn't forget requests but who knows. */
			atomic_sub(ap_dev->queue_count, &ap_poll_requests);
			ap_dev->queue_count = 0;
			list_splice_init(&ap_dev->pendingq, &ap_dev->requestq);
			ap_dev->requestq_count += ap_dev->pendingq_count;
			ap_dev->pendingq_count = 0;
		} else
			*flags |= 2;
		break;
	default:
		return -ENODEV;
	}
	return 0;
}
