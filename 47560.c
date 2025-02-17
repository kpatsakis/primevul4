_load_ucode_intel_bsp(struct mc_saved_data *mc_saved_data,
		      unsigned long *mc_saved_in_initrd,
		      unsigned long initrd_start_early,
		      unsigned long initrd_end_early,
		      struct ucode_cpu_info *uci)
{
	enum ucode_state ret;

	collect_cpu_info_early(uci);
	scan_microcode(initrd_start_early, initrd_end_early, mc_saved_data,
		       mc_saved_in_initrd, uci);

	ret = load_microcode(mc_saved_data, mc_saved_in_initrd,
			     initrd_start_early, uci);

	if (ret == UCODE_OK)
		apply_microcode_early(uci, true);
}
