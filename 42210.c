long vhost_dev_reset_owner(struct vhost_dev *dev)
{
	struct vhost_memory *memory;

	/* Restore memory to default empty mapping. */
	memory = kmalloc(offsetof(struct vhost_memory, regions), GFP_KERNEL);
	if (!memory)
		return -ENOMEM;

	vhost_dev_cleanup(dev, true);

	memory->nregions = 0;
	RCU_INIT_POINTER(dev->memory, memory);
	return 0;
}
