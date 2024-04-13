static void logi_dj_recv_forward_null_report(struct dj_receiver_dev *djrcv_dev,
					     struct dj_report *dj_report)
{
	/* We are called from atomic context (tasklet && djrcv->lock held) */
	unsigned int i;
	u8 reportbuffer[MAX_REPORT_SIZE];
	struct dj_device *djdev;

	djdev = djrcv_dev->paired_dj_devices[dj_report->device_index];

	if (!djdev) {
		dbg_hid("djrcv_dev->paired_dj_devices[dj_report->device_index]"
			" is NULL, index %d\n", dj_report->device_index);
		kfifo_in(&djrcv_dev->notif_fifo, dj_report, sizeof(struct dj_report));

		if (schedule_work(&djrcv_dev->work) == 0) {
			dbg_hid("%s: did not schedule the work item, was already "
			"queued\n", __func__);
		}
		return;
	}

	memset(reportbuffer, 0, sizeof(reportbuffer));

	for (i = 0; i < NUMBER_OF_HID_REPORTS; i++) {
		if (djdev->reports_supported & (1 << i)) {
			reportbuffer[0] = i;
			if (hid_input_report(djdev->hdev,
					     HID_INPUT_REPORT,
					     reportbuffer,
					     hid_reportid_size_map[i], 1)) {
				dbg_hid("hid_input_report error sending null "
					"report\n");
			}
		}
	}
}
