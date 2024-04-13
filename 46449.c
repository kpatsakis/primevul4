struct vhost_memory *vhost_dev_reset_owner_prepare(void)
{
	return kmalloc(offsetof(struct vhost_memory, regions), GFP_KERNEL);
}
