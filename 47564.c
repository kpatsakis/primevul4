generic_load_microcode_early(struct microcode_intel **mc_saved_p,
			     unsigned int mc_saved_count,
			     struct ucode_cpu_info *uci)
{
	struct microcode_intel *ucode_ptr, *new_mc = NULL;
	int new_rev = uci->cpu_sig.rev;
	enum ucode_state state = UCODE_OK;
	unsigned int mc_size;
	struct microcode_header_intel *mc_header;
	unsigned int csig = uci->cpu_sig.sig;
	unsigned int cpf = uci->cpu_sig.pf;
	int i;

	for (i = 0; i < mc_saved_count; i++) {
		ucode_ptr = mc_saved_p[i];

		mc_header = (struct microcode_header_intel *)ucode_ptr;
		mc_size = get_totalsize(mc_header);
		if (get_matching_microcode(csig, cpf, ucode_ptr, new_rev)) {
			new_rev = mc_header->rev;
			new_mc  = ucode_ptr;
		}
	}

	if (!new_mc) {
		state = UCODE_NFOUND;
		goto out;
	}

	uci->mc = (struct microcode_intel *)new_mc;
out:
	return state;
}
