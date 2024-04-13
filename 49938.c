static int ap_poll_write(struct ap_device *ap_dev, unsigned long *flags)
{
	struct ap_queue_status status;
	struct ap_message *ap_msg;

	if (ap_dev->requestq_count <= 0 ||
	    ap_dev->queue_count >= ap_dev->queue_depth)
		return 0;
	/* Start the next request on the queue. */
	ap_msg = list_entry(ap_dev->requestq.next, struct ap_message, list);
	status = __ap_send(ap_dev->qid, ap_msg->psmid,
			   ap_msg->message, ap_msg->length, ap_msg->special);
	switch (status.response_code) {
	case AP_RESPONSE_NORMAL:
		atomic_inc(&ap_poll_requests);
		ap_increase_queue_count(ap_dev);
		list_move_tail(&ap_msg->list, &ap_dev->pendingq);
		ap_dev->requestq_count--;
		ap_dev->pendingq_count++;
		if (ap_dev->queue_count < ap_dev->queue_depth &&
		    ap_dev->requestq_count > 0)
			*flags |= 1;
		*flags |= 2;
		break;
	case AP_RESPONSE_RESET_IN_PROGRESS:
		__ap_schedule_poll_timer();
	case AP_RESPONSE_Q_FULL:
		*flags |= 2;
		break;
	case AP_RESPONSE_MESSAGE_TOO_BIG:
	case AP_RESPONSE_REQ_FAC_NOT_INST:
		return -EINVAL;
	default:
		return -ENODEV;
	}
	return 0;
}
