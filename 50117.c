int snd_pcm_hw_rule_add(struct snd_pcm_runtime *runtime, unsigned int cond,
			int var,
			snd_pcm_hw_rule_func_t func, void *private,
			int dep, ...)
{
	struct snd_pcm_hw_constraints *constrs = &runtime->hw_constraints;
	struct snd_pcm_hw_rule *c;
	unsigned int k;
	va_list args;
	va_start(args, dep);
	if (constrs->rules_num >= constrs->rules_all) {
		struct snd_pcm_hw_rule *new;
		unsigned int new_rules = constrs->rules_all + 16;
		new = kcalloc(new_rules, sizeof(*c), GFP_KERNEL);
		if (!new) {
			va_end(args);
			return -ENOMEM;
		}
		if (constrs->rules) {
			memcpy(new, constrs->rules,
			       constrs->rules_num * sizeof(*c));
			kfree(constrs->rules);
		}
		constrs->rules = new;
		constrs->rules_all = new_rules;
	}
	c = &constrs->rules[constrs->rules_num];
	c->cond = cond;
	c->func = func;
	c->var = var;
	c->private = private;
	k = 0;
	while (1) {
		if (snd_BUG_ON(k >= ARRAY_SIZE(c->deps))) {
			va_end(args);
			return -EINVAL;
		}
		c->deps[k++] = dep;
		if (dep < 0)
			break;
		dep = va_arg(args, int);
	}
	constrs->rules_num++;
	va_end(args);
	return 0;
}
