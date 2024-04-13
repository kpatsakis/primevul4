static void set_nettime(void)
{
	wr_lock(&netacc_lock);
	cgtime(&nettime);
	wr_unlock(&netacc_lock);
}
