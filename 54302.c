snd_compr_tstamp(struct snd_compr_stream *stream, unsigned long arg)
{
	struct snd_compr_tstamp tstamp;

	snd_compr_update_tstamp(stream, &tstamp);
	return copy_to_user((struct snd_compr_tstamp __user *)arg,
		&tstamp, sizeof(tstamp)) ? -EFAULT : 0;
}
