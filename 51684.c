static int airspy_s_tuner(struct file *file, void *priv,
		const struct v4l2_tuner *v)
{
	int ret;

	if (v->index == 0)
		ret = 0;
	else if (v->index == 1)
		ret = 0;
	else
		ret = -EINVAL;

	return ret;
}
