static int oz_urb_process(struct oz_hcd *ozhcd, struct urb *urb)
{
	int rc = 0;
	struct oz_port *port = urb->hcpriv;
	u8 ep_addr;

	/* When we are paranoid we keep a list of urbs which we check against
	 * before handing one back. This is just for debugging during
	 * development and should be turned off in the released driver.
	 */
	oz_remember_urb(urb);
	/* Check buffer is valid.
	 */
	if (!urb->transfer_buffer && urb->transfer_buffer_length)
		return -EINVAL;
	/* Check if there is a device at the port - refuse if not.
	 */
	if ((port->flags & OZ_PORT_F_PRESENT) == 0)
		return -EPIPE;
	ep_addr = usb_pipeendpoint(urb->pipe);
	if (ep_addr) {
		/* If the request is not for EP0 then queue it.
		 */
		if (oz_enqueue_ep_urb(port, ep_addr, usb_pipein(urb->pipe),
			urb, 0))
			rc = -EPIPE;
	} else {
		oz_process_ep0_urb(ozhcd, urb, GFP_ATOMIC);
	}
	return rc;
}
