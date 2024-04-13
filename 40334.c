static bool fb_do_apertures_overlap(struct apertures_struct *gena,
				    struct apertures_struct *hwa)
{
	int i, j;
	if (!hwa || !gena)
		return false;

	for (i = 0; i < hwa->count; ++i) {
		struct aperture *h = &hwa->ranges[i];
		for (j = 0; j < gena->count; ++j) {
			struct aperture *g = &gena->ranges[j];
			printk(KERN_DEBUG "checking generic (%llx %llx) vs hw (%llx %llx)\n",
				(unsigned long long)g->base,
				(unsigned long long)g->size,
				(unsigned long long)h->base,
				(unsigned long long)h->size);
			if (apertures_overlap(g, h))
				return true;
		}
	}

	return false;
}
