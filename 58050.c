static int catc_ctrl_async(struct catc *catc, u8 dir, u8 request, u16 value,
	u16 index, void *buf, int len, void (*callback)(struct catc *catc, struct ctrl_queue *q))
{
	struct ctrl_queue *q;
	int retval = 0;
	unsigned long flags;

	spin_lock_irqsave(&catc->ctrl_lock, flags);
	
	q = catc->ctrl_queue + catc->ctrl_head;

	q->dir = dir;
	q->request = request;
	q->value = value;
	q->index = index;
	q->buf = buf;
	q->len = len;
	q->callback = callback;

	catc->ctrl_head = (catc->ctrl_head + 1) & (CTRL_QUEUE - 1);

	if (catc->ctrl_head == catc->ctrl_tail) {
		dev_err(&catc->usbdev->dev, "ctrl queue full\n");
		catc->ctrl_tail = (catc->ctrl_tail + 1) & (CTRL_QUEUE - 1);
		retval = -1;
	}

	if (!test_and_set_bit(CTRL_RUNNING, &catc->flags))
		catc_ctrl_run(catc);

	spin_unlock_irqrestore(&catc->ctrl_lock, flags);

	return retval;
}
