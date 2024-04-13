static void logi_dj_recv_destroy_djhid_device(struct dj_receiver_dev *djrcv_dev,
						struct dj_report *dj_report)
{
	/* Called in delayed work context */
	struct dj_device *dj_dev;
	unsigned long flags;

	spin_lock_irqsave(&djrcv_dev->lock, flags);
	dj_dev = djrcv_dev->paired_dj_devices[dj_report->device_index];
	djrcv_dev->paired_dj_devices[dj_report->device_index] = NULL;
	spin_unlock_irqrestore(&djrcv_dev->lock, flags);

	if (dj_dev != NULL) {
		hid_destroy_device(dj_dev->hdev);
		kfree(dj_dev);
	} else {
		dev_err(&djrcv_dev->hdev->dev, "%s: can't destroy a NULL device\n",
			__func__);
	}
}
