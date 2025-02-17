static void _save_mc(struct microcode_intel **mc_saved, u8 *ucode_ptr,
		     unsigned int *mc_saved_count_p)
{
	int i;
	int found = 0;
	unsigned int mc_saved_count = *mc_saved_count_p;
	struct microcode_header_intel *mc_header;

	mc_header = (struct microcode_header_intel *)ucode_ptr;
	for (i = 0; i < mc_saved_count; i++) {
		unsigned int sig, pf;
		unsigned int new_rev;
		struct microcode_header_intel *mc_saved_header =
			     (struct microcode_header_intel *)mc_saved[i];
		sig = mc_saved_header->sig;
		pf = mc_saved_header->pf;
		new_rev = mc_header->rev;

		if (get_matching_sig(sig, pf, ucode_ptr, new_rev)) {
			found = 1;
			if (update_match_revision(mc_header, new_rev)) {
				/*
				 * Found an older ucode saved before.
				 * Replace the older one with this newer
				 * one.
				 */
				mc_saved[i] =
					(struct microcode_intel *)ucode_ptr;
				break;
			}
		}
	}
	if (i >= mc_saved_count && !found)
		/*
		 * This ucode is first time discovered in ucode file.
		 * Save it to memory.
		 */
		mc_saved[mc_saved_count++] =
				 (struct microcode_intel *)ucode_ptr;

	*mc_saved_count_p = mc_saved_count;
}
