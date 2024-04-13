static int snd_pcm_hw_rule_msbits(struct snd_pcm_hw_params *params,
				  struct snd_pcm_hw_rule *rule)
{
	unsigned int l = (unsigned long) rule->private;
	int width = l & 0xffff;
	unsigned int msbits = l >> 16;
	struct snd_interval *i = hw_param_interval(params, SNDRV_PCM_HW_PARAM_SAMPLE_BITS);

	if (!snd_interval_single(i))
		return 0;

	if ((snd_interval_value(i) == width) ||
	    (width == 0 && snd_interval_value(i) > msbits))
		params->msbits = min_not_zero(params->msbits, msbits);

	return 0;
}
