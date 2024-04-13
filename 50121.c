static int snd_pcm_hw_rule_noresample_func(struct snd_pcm_hw_params *params,
					   struct snd_pcm_hw_rule *rule)
{
	unsigned int base_rate = (unsigned int)(uintptr_t)rule->private;
	struct snd_interval *rate;

	rate = hw_param_interval(params, SNDRV_PCM_HW_PARAM_RATE);
	return snd_interval_list(rate, 1, &base_rate, 0);
}
