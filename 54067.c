static long snd_compr_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	struct snd_compr_file *data = f->private_data;
	struct snd_compr_stream *stream;
	int retval = -ENOTTY;

	if (snd_BUG_ON(!data))
		return -EFAULT;
	stream = &data->stream;
	if (snd_BUG_ON(!stream))
		return -EFAULT;
	mutex_lock(&stream->device->lock);
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(SNDRV_COMPRESS_IOCTL_VERSION):
		retval = put_user(SNDRV_COMPRESS_VERSION,
				(int __user *)arg) ? -EFAULT : 0;
		break;
	case _IOC_NR(SNDRV_COMPRESS_GET_CAPS):
		retval = snd_compr_get_caps(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_GET_CODEC_CAPS):
		retval = snd_compr_get_codec_caps(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_SET_PARAMS):
		retval = snd_compr_set_params(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_GET_PARAMS):
		retval = snd_compr_get_params(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_SET_METADATA):
		retval = snd_compr_set_metadata(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_GET_METADATA):
		retval = snd_compr_get_metadata(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_TSTAMP):
		retval = snd_compr_tstamp(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_AVAIL):
		retval = snd_compr_ioctl_avail(stream, arg);
		break;
	case _IOC_NR(SNDRV_COMPRESS_PAUSE):
		retval = snd_compr_pause(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_RESUME):
		retval = snd_compr_resume(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_START):
		retval = snd_compr_start(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_STOP):
		retval = snd_compr_stop(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_DRAIN):
		retval = snd_compr_drain(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_PARTIAL_DRAIN):
		retval = snd_compr_partial_drain(stream);
		break;
	case _IOC_NR(SNDRV_COMPRESS_NEXT_TRACK):
		retval = snd_compr_next_track(stream);
		break;

	}
	mutex_unlock(&stream->device->lock);
	return retval;
}
