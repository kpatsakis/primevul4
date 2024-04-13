static int snd_pcm_lib_writev_transfer(struct snd_pcm_substream *substream,
				       unsigned int hwoff,
				       unsigned long data, unsigned int off,
				       snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;
	void __user **bufs = (void __user **)data;
	int channels = runtime->channels;
	int c;
	if (substream->ops->copy) {
		if (snd_BUG_ON(!substream->ops->silence))
			return -EINVAL;
		for (c = 0; c < channels; ++c, ++bufs) {
			if (*bufs == NULL) {
				if ((err = substream->ops->silence(substream, c, hwoff, frames)) < 0)
					return err;
			} else {
				char __user *buf = *bufs + samples_to_bytes(runtime, off);
				if ((err = substream->ops->copy(substream, c, hwoff, buf, frames)) < 0)
					return err;
			}
		}
	} else {
		/* default transfer behaviour */
		size_t dma_csize = runtime->dma_bytes / channels;
		for (c = 0; c < channels; ++c, ++bufs) {
			char *hwbuf = runtime->dma_area + (c * dma_csize) + samples_to_bytes(runtime, hwoff);
			if (*bufs == NULL) {
				snd_pcm_format_set_silence(runtime->format, hwbuf, frames);
			} else {
				char __user *buf = *bufs + samples_to_bytes(runtime, off);
				if (copy_from_user(hwbuf, buf, samples_to_bytes(runtime, frames)))
					return -EFAULT;
			}
		}
	}
	return 0;
}
