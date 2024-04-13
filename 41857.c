static int bulk_xfer(struct usb_serial *serial, void *buffer,
						int length, int *num_sent)
{
	int status;

	status = usb_bulk_msg(serial->dev,
			usb_sndbulkpipe(serial->dev,
				serial->port[0]->bulk_out_endpointAddress),
			buffer, length, num_sent, 1000);
	return status;
}
