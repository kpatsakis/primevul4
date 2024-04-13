static void acm_tty_flush_chars(struct tty_struct *tty)
{
	struct acm *acm = tty->driver_data;
	struct acm_wb *cur = acm->putbuffer;
	int err;
	unsigned long flags;

	acm->putbuffer = NULL;
	err = usb_autopm_get_interface_async(acm->control);
	spin_lock_irqsave(&acm->write_lock, flags);
	if (err < 0) {
		cur->use = 0;
		goto out;
	}

	if (acm->susp_count)
		usb_anchor_urb(cur->urb, &acm->delayed);
	else
		acm_start_wb(acm, cur);
out:
	spin_unlock_irqrestore(&acm->write_lock, flags);
	return;
}
