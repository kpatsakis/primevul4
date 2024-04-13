static void airspy_urb_complete(struct urb *urb)
{
	struct airspy *s = urb->context;
	struct airspy_frame_buf *fbuf;

	dev_dbg_ratelimited(s->dev, "status=%d length=%d/%d errors=%d\n",
			urb->status, urb->actual_length,
			urb->transfer_buffer_length, urb->error_count);

	switch (urb->status) {
	case 0:             /* success */
	case -ETIMEDOUT:    /* NAK */
		break;
	case -ECONNRESET:   /* kill */
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	default:            /* error */
		dev_err_ratelimited(s->dev, "URB failed %d\n", urb->status);
		break;
	}

	if (likely(urb->actual_length > 0)) {
		void *ptr;
		unsigned int len;
		/* get free framebuffer */
		fbuf = airspy_get_next_fill_buf(s);
		if (unlikely(fbuf == NULL)) {
			s->vb_full++;
			dev_notice_ratelimited(s->dev,
					"videobuf is full, %d packets dropped\n",
					s->vb_full);
			goto skip;
		}

		/* fill framebuffer */
		ptr = vb2_plane_vaddr(&fbuf->vb.vb2_buf, 0);
		len = airspy_convert_stream(s, ptr, urb->transfer_buffer,
				urb->actual_length);
		vb2_set_plane_payload(&fbuf->vb.vb2_buf, 0, len);
		fbuf->vb.vb2_buf.timestamp = ktime_get_ns();
		fbuf->vb.sequence = s->sequence++;
		vb2_buffer_done(&fbuf->vb.vb2_buf, VB2_BUF_STATE_DONE);
	}
skip:
	usb_submit_urb(urb, GFP_ATOMIC);
}
