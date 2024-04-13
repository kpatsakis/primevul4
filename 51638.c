static void hiddev_send_event(struct hid_device *hid,
			      struct hiddev_usage_ref *uref)
{
	struct hiddev *hiddev = hid->hiddev;
	struct hiddev_list *list;
	unsigned long flags;

	spin_lock_irqsave(&hiddev->list_lock, flags);
	list_for_each_entry(list, &hiddev->list, node) {
		if (uref->field_index != HID_FIELD_INDEX_NONE ||
		    (list->flags & HIDDEV_FLAG_REPORT) != 0) {
			list->buffer[list->head] = *uref;
			list->head = (list->head + 1) &
				(HIDDEV_BUFFER_SIZE - 1);
			kill_fasync(&list->fasync, SIGIO, POLL_IN);
		}
	}
	spin_unlock_irqrestore(&hiddev->list_lock, flags);

	wake_up_interruptible(&hiddev->wait);
}
