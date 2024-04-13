static int download_code(struct edgeport_serial *serial, __u8 *image,
							int image_length)
{
	int status = 0;
	int pos;
	int transfer;
	int done;

	/* Transfer firmware image */
	for (pos = 0; pos < image_length; ) {
		/* Read the next buffer from file */
		transfer = image_length - pos;
		if (transfer > EDGE_FW_BULK_MAX_PACKET_SIZE)
			transfer = EDGE_FW_BULK_MAX_PACKET_SIZE;

		/* Transfer data */
		status = bulk_xfer(serial->serial, &image[pos],
							transfer, &done);
		if (status)
			break;
		/* Advance buffer pointer */
		pos += done;
	}

	return status;
}
