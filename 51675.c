static int airspy_g_fmt_sdr_cap(struct file *file, void *priv,
		struct v4l2_format *f)
{
	struct airspy *s = video_drvdata(file);

	f->fmt.sdr.pixelformat = s->pixelformat;
	f->fmt.sdr.buffersize = s->buffersize;
	memset(f->fmt.sdr.reserved, 0, sizeof(f->fmt.sdr.reserved));

	return 0;
}
