static int check_val_ptr_alignment(const struct bpf_reg_state *reg,
				   int size)
{
	if (size != 1) {
		verbose("Unknown alignment. Only byte-sized access allowed in value access.\n");
		return -EACCES;
	}

	return 0;
}
