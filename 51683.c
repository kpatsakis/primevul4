static int airspy_s_fmt_sdr_cap(struct file *file, void *priv,
		struct v4l2_format *f)
{
	struct airspy *s = video_drvdata(file);
	struct vb2_queue *q = &s->vb_queue;
	int i;

	if (vb2_is_busy(q))
		return -EBUSY;

	memset(f->fmt.sdr.reserved, 0, sizeof(f->fmt.sdr.reserved));
	for (i = 0; i < NUM_FORMATS; i++) {
		if (formats[i].pixelformat == f->fmt.sdr.pixelformat) {
			s->pixelformat = formats[i].pixelformat;
			s->buffersize = formats[i].buffersize;
			f->fmt.sdr.buffersize = formats[i].buffersize;
			return 0;
		}
	}

	s->pixelformat = formats[0].pixelformat;
	s->buffersize = formats[0].buffersize;
	f->fmt.sdr.pixelformat = formats[0].pixelformat;
	f->fmt.sdr.buffersize = formats[0].buffersize;

	return 0;
}
