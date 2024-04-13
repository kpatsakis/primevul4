static void fanout_init_data(struct packet_fanout *f)
{
	switch (f->type) {
	case PACKET_FANOUT_LB:
		atomic_set(&f->rr_cur, 0);
		break;
	case PACKET_FANOUT_CBPF:
	case PACKET_FANOUT_EBPF:
		RCU_INIT_POINTER(f->bpf_prog, NULL);
		break;
	}
}
