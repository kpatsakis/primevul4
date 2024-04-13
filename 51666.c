static int airspy_alloc_stream_bufs(struct airspy *s)
{
	s->buf_num = 0;
	s->buf_size = BULK_BUFFER_SIZE;

	dev_dbg(s->dev, "all in all I will use %u bytes for streaming\n",
			MAX_BULK_BUFS * BULK_BUFFER_SIZE);

	for (s->buf_num = 0; s->buf_num < MAX_BULK_BUFS; s->buf_num++) {
		s->buf_list[s->buf_num] = usb_alloc_coherent(s->udev,
				BULK_BUFFER_SIZE, GFP_ATOMIC,
				&s->dma_addr[s->buf_num]);
		if (!s->buf_list[s->buf_num]) {
			dev_dbg(s->dev, "alloc buf=%d failed\n", s->buf_num);
			airspy_free_stream_bufs(s);
			return -ENOMEM;
		}

		dev_dbg(s->dev, "alloc buf=%d %p (dma %llu)\n", s->buf_num,
				s->buf_list[s->buf_num],
				(long long)s->dma_addr[s->buf_num]);
		set_bit(USB_STATE_URB_BUF, &s->flags);
	}

	return 0;
}
