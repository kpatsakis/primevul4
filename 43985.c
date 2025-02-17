static void logi_dj_recv_forward_report(struct dj_receiver_dev *djrcv_dev,
					struct dj_report *dj_report)
{
	/* We are called from atomic context (tasklet && djrcv->lock held) */
	struct dj_device *dj_device;

	dj_device = djrcv_dev->paired_dj_devices[dj_report->device_index];

	if (dj_device == NULL) {
		dbg_hid("djrcv_dev->paired_dj_devices[dj_report->device_index]"
			" is NULL, index %d\n", dj_report->device_index);
		kfifo_in(&djrcv_dev->notif_fifo, dj_report, sizeof(struct dj_report));

		if (schedule_work(&djrcv_dev->work) == 0) {
			dbg_hid("%s: did not schedule the work item, was already "
			"queued\n", __func__);
		}
		return;
	}

	if ((dj_report->report_type > ARRAY_SIZE(hid_reportid_size_map) - 1) ||
	    (hid_reportid_size_map[dj_report->report_type] == 0)) {
		dbg_hid("invalid report type:%x\n", dj_report->report_type);
		return;
	}

	if (hid_input_report(dj_device->hdev,
			HID_INPUT_REPORT, &dj_report->report_type,
			hid_reportid_size_map[dj_report->report_type], 1)) {
		dbg_hid("hid_input_report error\n");
	}
}
