static int airspy_enum_freq_bands(struct file *file, void *priv,
		struct v4l2_frequency_band *band)
{
	int ret;

	if (band->tuner == 0) {
		if (band->index >= ARRAY_SIZE(bands)) {
			ret = -EINVAL;
		} else {
			*band = bands[band->index];
			ret = 0;
		}
	} else if (band->tuner == 1) {
		if (band->index >= ARRAY_SIZE(bands_rf)) {
			ret = -EINVAL;
		} else {
			*band = bands_rf[band->index];
			ret = 0;
		}
	} else {
		ret = -EINVAL;
	}

	return ret;
}
