static int snd_interval_step(struct snd_interval *i, unsigned int step)
{
	unsigned int n;
	int changed = 0;
	n = i->min % step;
	if (n != 0 || i->openmin) {
		i->min += step - n;
		i->openmin = 0;
		changed = 1;
	}
	n = i->max % step;
	if (n != 0 || i->openmax) {
		i->max -= n;
		i->openmax = 0;
		changed = 1;
	}
	if (snd_interval_checkempty(i)) {
		i->empty = 1;
		return -EINVAL;
	}
	return changed;
}
