static int ti_cpu_rev(struct edge_ti_manuf_descriptor *desc)
{
	return TI_GET_CPU_REVISION(desc->CpuRev_BoardRev);
}
