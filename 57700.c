static int check_pkt_ptr_alignment(const struct bpf_reg_state *reg,
				   int off, int size)
{
	if (reg->id && size != 1) {
		verbose("Unknown alignment. Only byte-sized access allowed in packet access.\n");
		return -EACCES;
	}

	/* skb->data is NET_IP_ALIGN-ed */
	if ((NET_IP_ALIGN + reg->off + off) % size != 0) {
		verbose("misaligned packet access off %d+%d+%d size %d\n",
			NET_IP_ALIGN, reg->off, off, size);
		return -EACCES;
	}

	return 0;
}
