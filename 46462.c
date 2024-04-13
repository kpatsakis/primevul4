static int vhost_memory_reg_sort_cmp(const void *p1, const void *p2)
{
	const struct vhost_memory_region *r1 = p1, *r2 = p2;
	if (r1->guest_phys_addr < r2->guest_phys_addr)
		return 1;
	if (r1->guest_phys_addr > r2->guest_phys_addr)
		return -1;
	return 0;
}
