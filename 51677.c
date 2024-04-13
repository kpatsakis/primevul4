static int airspy_g_tuner(struct file *file, void *priv, struct v4l2_tuner *v)
{
	int ret;

	if (v->index == 0) {
		strlcpy(v->name, "AirSpy ADC", sizeof(v->name));
		v->type = V4L2_TUNER_ADC;
		v->capability = V4L2_TUNER_CAP_1HZ | V4L2_TUNER_CAP_FREQ_BANDS;
		v->rangelow  = bands[0].rangelow;
		v->rangehigh = bands[0].rangehigh;
		ret = 0;
	} else if (v->index == 1) {
		strlcpy(v->name, "AirSpy RF", sizeof(v->name));
		v->type = V4L2_TUNER_RF;
		v->capability = V4L2_TUNER_CAP_1HZ | V4L2_TUNER_CAP_FREQ_BANDS;
		v->rangelow  = bands_rf[0].rangelow;
		v->rangehigh = bands_rf[0].rangehigh;
		ret = 0;
	} else {
		ret = -EINVAL;
	}

	return ret;
}
