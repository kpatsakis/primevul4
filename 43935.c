struct picolcd_pending *picolcd_send_and_wait(struct hid_device *hdev,
		int report_id, const u8 *raw_data, int size)
{
	struct picolcd_data *data = hid_get_drvdata(hdev);
	struct picolcd_pending *work;
	struct hid_report *report = picolcd_out_report(report_id, hdev);
	unsigned long flags;
	int i, j, k;

	if (!report || !data)
		return NULL;
	if (data->status & PICOLCD_FAILED)
		return NULL;
	work = kzalloc(sizeof(*work), GFP_KERNEL);
	if (!work)
		return NULL;

	init_completion(&work->ready);
	work->out_report = report;
	work->in_report  = NULL;
	work->raw_size   = 0;

	mutex_lock(&data->mutex);
	spin_lock_irqsave(&data->lock, flags);
	for (i = k = 0; i < report->maxfield; i++)
		for (j = 0; j < report->field[i]->report_count; j++) {
			hid_set_field(report->field[i], j, k < size ? raw_data[k] : 0);
			k++;
		}
	if (data->status & PICOLCD_FAILED) {
		kfree(work);
		work = NULL;
	} else {
		data->pending = work;
		hid_hw_request(data->hdev, report, HID_REQ_SET_REPORT);
		spin_unlock_irqrestore(&data->lock, flags);
		wait_for_completion_interruptible_timeout(&work->ready, HZ*2);
		spin_lock_irqsave(&data->lock, flags);
		data->pending = NULL;
	}
	spin_unlock_irqrestore(&data->lock, flags);
	mutex_unlock(&data->mutex);
	return work;
}
