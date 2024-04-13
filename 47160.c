static void oz_complete_buffered_urb(struct oz_port *port,
			struct oz_endpoint *ep,
			struct urb *urb)
{
	int data_len, available_space, copy_len;

	data_len = ep->buffer[ep->out_ix];
	if (data_len <= urb->transfer_buffer_length)
		available_space = data_len;
	else
		available_space = urb->transfer_buffer_length;

	if (++ep->out_ix == ep->buffer_size)
		ep->out_ix = 0;
	copy_len = ep->buffer_size - ep->out_ix;
	if (copy_len >= available_space)
		copy_len = available_space;
	memcpy(urb->transfer_buffer, &ep->buffer[ep->out_ix], copy_len);

	if (copy_len < available_space) {
		memcpy((urb->transfer_buffer + copy_len), ep->buffer,
						(available_space - copy_len));
		ep->out_ix = available_space - copy_len;
	} else {
		ep->out_ix += copy_len;
	}
	urb->actual_length = available_space;
	if (ep->out_ix == ep->buffer_size)
		ep->out_ix = 0;

	ep->buffered_units--;
	oz_dbg(ON, "Trying to give back buffered frame of size=%d\n",
	       available_space);
	oz_complete_urb(port->ozhcd->hcd, urb, 0);
}
