static const struct vhost_memory_region *find_region(struct vhost_memory *mem,
						     __u64 addr, __u32 len)
{
	const struct vhost_memory_region *reg;
	int start = 0, end = mem->nregions;

	while (start < end) {
		int slot = start + (end - start) / 2;
		reg = mem->regions + slot;
		if (addr >= reg->guest_phys_addr)
			end = slot;
		else
			start = slot + 1;
	}

	reg = mem->regions + start;
	if (addr >= reg->guest_phys_addr &&
		reg->guest_phys_addr + reg->memory_size > addr)
		return reg;
	return NULL;
}
