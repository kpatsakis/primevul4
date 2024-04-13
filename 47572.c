static void print_ucode(struct ucode_cpu_info *uci)
{
	struct microcode_intel *mc_intel;
	int *delay_ucode_info_p;
	int *current_mc_date_p;

	mc_intel = uci->mc;
	if (mc_intel == NULL)
		return;

	delay_ucode_info_p = (int *)__pa_nodebug(&delay_ucode_info);
	current_mc_date_p = (int *)__pa_nodebug(&current_mc_date);

	*delay_ucode_info_p = 1;
	*current_mc_date_p = mc_intel->hdr.date;
}
