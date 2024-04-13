inline struct dl_bw *dl_bw_of(int i)
{
	return &cpu_rq(i)->rd->dl_bw;
}
