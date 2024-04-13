static int softnet_seq_show(struct seq_file *seq, void *v)
{
	struct softnet_data *sd = v;

	seq_printf(seq, "%08x %08x %08x %08x %08x %08x %08x %08x %08x %08x\n",
		   sd->processed, sd->dropped, sd->time_squeeze, 0,
		   0, 0, 0, 0, /* was fastroute */
		   sd->cpu_collision, sd->received_rps);
	return 0;
}
