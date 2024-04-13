save_microcode(struct mc_saved_data *mc_saved_data,
	       struct microcode_intel **mc_saved_src,
	       unsigned int mc_saved_count)
{
	int i, j;
	struct microcode_intel **mc_saved_p;
	int ret;

	if (!mc_saved_count)
		return -EINVAL;

	/*
	 * Copy new microcode data.
	 */
	mc_saved_p = kmalloc(mc_saved_count*sizeof(struct microcode_intel *),
			     GFP_KERNEL);
	if (!mc_saved_p)
		return -ENOMEM;

	for (i = 0; i < mc_saved_count; i++) {
		struct microcode_intel *mc = mc_saved_src[i];
		struct microcode_header_intel *mc_header = &mc->hdr;
		unsigned long mc_size = get_totalsize(mc_header);
		mc_saved_p[i] = kmalloc(mc_size, GFP_KERNEL);
		if (!mc_saved_p[i]) {
			ret = -ENOMEM;
			goto err;
		}
		if (!mc_saved_src[i]) {
			ret = -EINVAL;
			goto err;
		}
		memcpy(mc_saved_p[i], mc, mc_size);
	}

	/*
	 * Point to newly saved microcode.
	 */
	mc_saved_data->mc_saved = mc_saved_p;
	mc_saved_data->mc_saved_count = mc_saved_count;

	return 0;

err:
	for (j = 0; j <= i; j++)
		kfree(mc_saved_p[j]);
	kfree(mc_saved_p);

	return ret;
}
