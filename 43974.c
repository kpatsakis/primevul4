static void delayedwork_callback(struct work_struct *work)
{
	struct dj_receiver_dev *djrcv_dev =
		container_of(work, struct dj_receiver_dev, work);

	struct dj_report dj_report;
	unsigned long flags;
	int count;
	int retval;

	dbg_hid("%s\n", __func__);

	spin_lock_irqsave(&djrcv_dev->lock, flags);

	count = kfifo_out(&djrcv_dev->notif_fifo, &dj_report,
				sizeof(struct dj_report));

	if (count != sizeof(struct dj_report)) {
		dev_err(&djrcv_dev->hdev->dev, "%s: workitem triggered without "
			"notifications available\n", __func__);
		spin_unlock_irqrestore(&djrcv_dev->lock, flags);
		return;
	}

	if (!kfifo_is_empty(&djrcv_dev->notif_fifo)) {
		if (schedule_work(&djrcv_dev->work) == 0) {
			dbg_hid("%s: did not schedule the work item, was "
				"already queued\n", __func__);
		}
	}

	spin_unlock_irqrestore(&djrcv_dev->lock, flags);

	switch (dj_report.report_type) {
	case REPORT_TYPE_NOTIF_DEVICE_PAIRED:
		logi_dj_recv_add_djhid_device(djrcv_dev, &dj_report);
		break;
	case REPORT_TYPE_NOTIF_DEVICE_UNPAIRED:
		logi_dj_recv_destroy_djhid_device(djrcv_dev, &dj_report);
		break;
	default:
	/* A normal report (i. e. not belonging to a pair/unpair notification)
	 * arriving here, means that the report arrived but we did not have a
	 * paired dj_device associated to the report's device_index, this
	 * means that the original "device paired" notification corresponding
	 * to this dj_device never arrived to this driver. The reason is that
	 * hid-core discards all packets coming from a device while probe() is
	 * executing. */
	if (!djrcv_dev->paired_dj_devices[dj_report.device_index]) {
		/* ok, we don't know the device, just re-ask the
		 * receiver for the list of connected devices. */
		retval = logi_dj_recv_query_paired_devices(djrcv_dev);
		if (!retval) {
			/* everything went fine, so just leave */
			break;
		}
		dev_err(&djrcv_dev->hdev->dev,
			"%s:logi_dj_recv_query_paired_devices "
			"error:%d\n", __func__, retval);
		}
		dbg_hid("%s: unexpected report type\n", __func__);
	}
}
