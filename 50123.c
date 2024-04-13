static int snd_pcm_hw_rule_ranges(struct snd_pcm_hw_params *params,
				  struct snd_pcm_hw_rule *rule)
{
	struct snd_pcm_hw_constraint_ranges *r = rule->private;
	return snd_interval_ranges(hw_param_interval(params, rule->var),
				   r->count, r->ranges, r->mask);
}
