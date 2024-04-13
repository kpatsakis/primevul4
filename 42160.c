static int hidp_get_raw_report(struct hid_device *hid,
		unsigned char report_number,
		unsigned char *data, size_t count,
		unsigned char report_type)
{
	struct hidp_session *session = hid->driver_data;
	struct sk_buff *skb;
	size_t len;
	int numbered_reports = hid->report_enum[report_type].numbered;
	int ret;

	switch (report_type) {
	case HID_FEATURE_REPORT:
		report_type = HIDP_TRANS_GET_REPORT | HIDP_DATA_RTYPE_FEATURE;
		break;
	case HID_INPUT_REPORT:
		report_type = HIDP_TRANS_GET_REPORT | HIDP_DATA_RTYPE_INPUT;
		break;
	case HID_OUTPUT_REPORT:
		report_type = HIDP_TRANS_GET_REPORT | HIDP_DATA_RTYPE_OUPUT;
		break;
	default:
		return -EINVAL;
	}

	if (mutex_lock_interruptible(&session->report_mutex))
		return -ERESTARTSYS;

	/* Set up our wait, and send the report request to the device. */
	session->waiting_report_type = report_type & HIDP_DATA_RTYPE_MASK;
	session->waiting_report_number = numbered_reports ? report_number : -1;
	set_bit(HIDP_WAITING_FOR_RETURN, &session->flags);
	data[0] = report_number;
	ret = hidp_send_ctrl_message(hid->driver_data, report_type, data, 1);
	if (ret)
		goto err;

	/* Wait for the return of the report. The returned report
	   gets put in session->report_return.  */
	while (test_bit(HIDP_WAITING_FOR_RETURN, &session->flags)) {
		int res;

		res = wait_event_interruptible_timeout(session->report_queue,
			!test_bit(HIDP_WAITING_FOR_RETURN, &session->flags),
			5*HZ);
		if (res == 0) {
			/* timeout */
			ret = -EIO;
			goto err;
		}
		if (res < 0) {
			/* signal */
			ret = -ERESTARTSYS;
			goto err;
		}
	}

	skb = session->report_return;
	if (skb) {
		len = skb->len < count ? skb->len : count;
		memcpy(data, skb->data, len);

		kfree_skb(skb);
		session->report_return = NULL;
	} else {
		/* Device returned a HANDSHAKE, indicating  protocol error. */
		len = -EIO;
	}

	clear_bit(HIDP_WAITING_FOR_RETURN, &session->flags);
	mutex_unlock(&session->report_mutex);

	return len;

err:
	clear_bit(HIDP_WAITING_FOR_RETURN, &session->flags);
	mutex_unlock(&session->report_mutex);
	return ret;
}
