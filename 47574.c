void reload_ucode_intel(void)
{
	struct ucode_cpu_info uci;
	enum ucode_state ret;

	if (!mc_saved_data.mc_saved_count)
		return;

	collect_cpu_info_early(&uci);

	ret = generic_load_microcode_early(mc_saved_data.mc_saved,
					   mc_saved_data.mc_saved_count, &uci);
	if (ret != UCODE_OK)
		return;

	apply_microcode_early(&uci, false);
}
