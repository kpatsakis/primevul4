static int check_ptr_alignment(const struct bpf_reg_state *reg,
			       int off, int size)
{
	switch (reg->type) {
	case PTR_TO_PACKET:
		return IS_ENABLED(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) ? 0 :
		       check_pkt_ptr_alignment(reg, off, size);
	case PTR_TO_MAP_VALUE_ADJ:
		return IS_ENABLED(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS) ? 0 :
		       check_val_ptr_alignment(reg, size);
	default:
		if (off % size != 0) {
			verbose("misaligned access off %d size %d\n",
				off, size);
			return -EACCES;
		}

		return 0;
	}
}
