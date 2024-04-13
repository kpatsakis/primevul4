static void logi_dj_remove(struct hid_device *hdev)
{
	struct dj_receiver_dev *djrcv_dev = hid_get_drvdata(hdev);
	struct dj_device *dj_dev;
	int i;

	dbg_hid("%s\n", __func__);

	cancel_work_sync(&djrcv_dev->work);

	hid_hw_close(hdev);
	hid_hw_stop(hdev);

	/* I suppose that at this point the only context that can access
	 * the djrecv_data is this thread as the work item is guaranteed to
	 * have finished and no more raw_event callbacks should arrive after
	 * the remove callback was triggered so no locks are put around the
	 * code below */
	for (i = 0; i < (DJ_MAX_PAIRED_DEVICES + DJ_DEVICE_INDEX_MIN); i++) {
		dj_dev = djrcv_dev->paired_dj_devices[i];
		if (dj_dev != NULL) {
			hid_destroy_device(dj_dev->hdev);
			kfree(dj_dev);
			djrcv_dev->paired_dj_devices[i] = NULL;
		}
	}

	kfifo_free(&djrcv_dev->notif_fifo);
	kfree(djrcv_dev);
	hid_set_drvdata(hdev, NULL);
}
