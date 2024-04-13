static int snd_pcm_lib_write_transfer(struct snd_pcm_substream *substream,
				      unsigned int hwoff,
				      unsigned long data, unsigned int off,
				      snd_pcm_uframes_t frames)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;
	char __user *buf = (char __user *) data + frames_to_bytes(runtime, off);
	if (substream->ops->copy) {
		if ((err = substream->ops->copy(substream, -1, hwoff, buf, frames)) < 0)
			return err;
	} else {
		char *hwbuf = runtime->dma_area + frames_to_bytes(runtime, hwoff);
		if (copy_from_user(hwbuf, buf, frames_to_bytes(runtime, frames)))
			return -EFAULT;
	}
	return 0;
}
