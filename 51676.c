static int airspy_g_frequency(struct file *file, void *priv,
		struct v4l2_frequency *f)
{
	struct airspy *s = video_drvdata(file);
	int ret;

	if (f->tuner == 0) {
		f->type = V4L2_TUNER_ADC;
		f->frequency = s->f_adc;
		dev_dbg(s->dev, "ADC frequency=%u Hz\n", s->f_adc);
		ret = 0;
	} else if (f->tuner == 1) {
		f->type = V4L2_TUNER_RF;
		f->frequency = s->f_rf;
		dev_dbg(s->dev, "RF frequency=%u Hz\n", s->f_rf);
		ret = 0;
	} else {
		ret = -EINVAL;
	}

	return ret;
}
