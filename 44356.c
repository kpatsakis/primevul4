static inline void release_mc(struct kref *kref)
{
	struct cma_multicast *mc = container_of(kref, struct cma_multicast, mcref);

	kfree(mc->multicast.ib);
	kfree(mc);
}
