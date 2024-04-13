static void command_port_write_callback(struct urb *urb)
{
	int status = urb->status;

	if (status) {
		dev_dbg(&urb->dev->dev, "nonzero urb status: %d\n", status);
		return;
	}
}
