static unsigned next_desc(struct vhost_virtqueue *vq, struct vring_desc *desc)
{
	unsigned int next;

	/* If this descriptor says it doesn't chain, we're done. */
	if (!(desc->flags & cpu_to_vhost16(vq, VRING_DESC_F_NEXT)))
		return -1U;

	/* Check they're not leading us off end of descriptors. */
	next = vhost16_to_cpu(vq, desc->next);
	/* Make sure compiler knows to grab that: we don't want it changing! */
	/* We will use the result as an index in an array, so most
	 * architectures only need a compiler barrier here. */
	read_barrier_depends();

	return next;
}
