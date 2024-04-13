static void __ref show_saved_mc(void)
{
	int i, j;
	unsigned int sig, pf, rev, total_size, data_size, date;
	struct ucode_cpu_info uci;

	if (mc_saved_data.mc_saved_count == 0) {
		pr_debug("no microcode data saved.\n");
		return;
	}
	pr_debug("Total microcode saved: %d\n", mc_saved_data.mc_saved_count);

	collect_cpu_info_early(&uci);

	sig = uci.cpu_sig.sig;
	pf = uci.cpu_sig.pf;
	rev = uci.cpu_sig.rev;
	pr_debug("CPU%d: sig=0x%x, pf=0x%x, rev=0x%x\n",
		 smp_processor_id(), sig, pf, rev);

	for (i = 0; i < mc_saved_data.mc_saved_count; i++) {
		struct microcode_header_intel *mc_saved_header;
		struct extended_sigtable *ext_header;
		int ext_sigcount;
		struct extended_signature *ext_sig;

		mc_saved_header = (struct microcode_header_intel *)
				  mc_saved_data.mc_saved[i];
		sig = mc_saved_header->sig;
		pf = mc_saved_header->pf;
		rev = mc_saved_header->rev;
		total_size = get_totalsize(mc_saved_header);
		data_size = get_datasize(mc_saved_header);
		date = mc_saved_header->date;

		pr_debug("mc_saved[%d]: sig=0x%x, pf=0x%x, rev=0x%x, toal size=0x%x, date = %04x-%02x-%02x\n",
			 i, sig, pf, rev, total_size,
			 date & 0xffff,
			 date >> 24,
			 (date >> 16) & 0xff);

		/* Look for ext. headers: */
		if (total_size <= data_size + MC_HEADER_SIZE)
			continue;

		ext_header = (struct extended_sigtable *)
			     mc_saved_header + data_size + MC_HEADER_SIZE;
		ext_sigcount = ext_header->count;
		ext_sig = (void *)ext_header + EXT_HEADER_SIZE;

		for (j = 0; j < ext_sigcount; j++) {
			sig = ext_sig->sig;
			pf = ext_sig->pf;

			pr_debug("\tExtended[%d]: sig=0x%x, pf=0x%x\n",
				 j, sig, pf);

			ext_sig++;
		}

	}
}
