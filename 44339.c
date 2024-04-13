static void iboe_mcast_work_handler(struct work_struct *work)
{
	struct iboe_mcast_work *mw = container_of(work, struct iboe_mcast_work, work);
	struct cma_multicast *mc = mw->mc;
	struct ib_sa_multicast *m = mc->multicast.ib;

	mc->multicast.ib->context = mc;
	cma_ib_mc_handler(0, m);
	kref_put(&mc->mcref, release_mc);
	kfree(mw);
}
