int __init save_microcode_in_initrd_intel(void)
{
	unsigned int count = mc_saved_data.mc_saved_count;
	struct microcode_intel *mc_saved[MAX_UCODE_COUNT];
	int ret = 0;

	if (count == 0)
		return ret;

	microcode_pointer(mc_saved, mc_saved_in_initrd, initrd_start, count);
	ret = save_microcode(&mc_saved_data, mc_saved, count);
	if (ret)
		pr_err("Cannot save microcode patches from initrd.\n");

	show_saved_mc();

	return ret;
}
