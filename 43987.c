static void logi_dj_recv_queue_notification(struct dj_receiver_dev *djrcv_dev,
					   struct dj_report *dj_report)
{
	/* We are called from atomic context (tasklet && djrcv->lock held) */

	kfifo_in(&djrcv_dev->notif_fifo, dj_report, sizeof(struct dj_report));

	if (schedule_work(&djrcv_dev->work) == 0) {
		dbg_hid("%s: did not schedule the work item, was already "
			"queued\n", __func__);
	}
}
