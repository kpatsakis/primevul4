static int airspy_free_stream_bufs(struct airspy *s)
{
	if (test_bit(USB_STATE_URB_BUF, &s->flags)) {
		while (s->buf_num) {
			s->buf_num--;
			dev_dbg(s->dev, "free buf=%d\n", s->buf_num);
			usb_free_coherent(s->udev, s->buf_size,
					  s->buf_list[s->buf_num],
					  s->dma_addr[s->buf_num]);
		}
	}
	clear_bit(USB_STATE_URB_BUF, &s->flags);

	return 0;
}
