static int airspy_alloc_urbs(struct airspy *s)
{
	int i, j;

	/* allocate the URBs */
	for (i = 0; i < MAX_BULK_BUFS; i++) {
		dev_dbg(s->dev, "alloc urb=%d\n", i);
		s->urb_list[i] = usb_alloc_urb(0, GFP_ATOMIC);
		if (!s->urb_list[i]) {
			dev_dbg(s->dev, "failed\n");
			for (j = 0; j < i; j++)
				usb_free_urb(s->urb_list[j]);
			return -ENOMEM;
		}
		usb_fill_bulk_urb(s->urb_list[i],
				s->udev,
				usb_rcvbulkpipe(s->udev, 0x81),
				s->buf_list[i],
				BULK_BUFFER_SIZE,
				airspy_urb_complete, s);

		s->urb_list[i]->transfer_flags = URB_NO_TRANSFER_DMA_MAP;
		s->urb_list[i]->transfer_dma = s->dma_addr[i];
		s->urbs_initialized++;
	}

	return 0;
}
