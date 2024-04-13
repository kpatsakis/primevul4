static void wdm_out_callback(struct urb *urb)
{
	struct wdm_device *desc;
	desc = urb->context;
	spin_lock(&desc->iuspin);
	desc->werr = urb->status;
	spin_unlock(&desc->iuspin);
	kfree(desc->outbuf);
	desc->outbuf = NULL;
	clear_bit(WDM_IN_USE, &desc->flags);
	wake_up(&desc->wait);
}
