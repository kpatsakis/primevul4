microcode_phys(struct microcode_intel **mc_saved_tmp,
	       struct mc_saved_data *mc_saved_data)
{
	int i;
	struct microcode_intel ***mc_saved;

	mc_saved = (struct microcode_intel ***)
		   __pa_nodebug(&mc_saved_data->mc_saved);
	for (i = 0; i < mc_saved_data->mc_saved_count; i++) {
		struct microcode_intel *p;

		p = *(struct microcode_intel **)
			__pa_nodebug(mc_saved_data->mc_saved + i);
		mc_saved_tmp[i] = (struct microcode_intel *)__pa_nodebug(p);
	}
}
