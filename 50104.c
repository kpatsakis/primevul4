void snd_pcm_debug_name(struct snd_pcm_substream *substream,
			   char *name, size_t len)
{
	snprintf(name, len, "pcmC%dD%d%c:%d",
		 substream->pcm->card->number,
		 substream->pcm->device,
		 substream->stream ? 'c' : 'p',
		 substream->number);
}
