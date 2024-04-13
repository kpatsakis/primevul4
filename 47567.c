load_microcode(struct mc_saved_data *mc_saved_data,
	       unsigned long *mc_saved_in_initrd,
	       unsigned long initrd_start,
	       struct ucode_cpu_info *uci)
{
	struct microcode_intel *mc_saved_tmp[MAX_UCODE_COUNT];
	unsigned int count = mc_saved_data->mc_saved_count;

	if (!mc_saved_data->mc_saved) {
		microcode_pointer(mc_saved_tmp, mc_saved_in_initrd,
				  initrd_start, count);

		return generic_load_microcode_early(mc_saved_tmp, count, uci);
	} else {
#ifdef CONFIG_X86_32
		microcode_phys(mc_saved_tmp, mc_saved_data);
		return generic_load_microcode_early(mc_saved_tmp, count, uci);
#else
		return generic_load_microcode_early(mc_saved_data->mc_saved,
						    count, uci);
#endif
	}
}
