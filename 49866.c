static int hash_get_device_data(struct hash_ctx *ctx,
				struct hash_device_data **device_data)
{
	int			ret;
	struct klist_iter	device_iterator;
	struct klist_node	*device_node;
	struct hash_device_data *local_device_data = NULL;

	/* Wait until a device is available */
	ret = down_interruptible(&driver_data.device_allocation);
	if (ret)
		return ret;  /* Interrupted */

	/* Select a device */
	klist_iter_init(&driver_data.device_list, &device_iterator);
	device_node = klist_next(&device_iterator);
	while (device_node) {
		local_device_data = container_of(device_node,
					   struct hash_device_data, list_node);
		spin_lock(&local_device_data->ctx_lock);
		/* current_ctx allocates a device, NULL = unallocated */
		if (local_device_data->current_ctx) {
			device_node = klist_next(&device_iterator);
		} else {
			local_device_data->current_ctx = ctx;
			ctx->device = local_device_data;
			spin_unlock(&local_device_data->ctx_lock);
			break;
		}
		spin_unlock(&local_device_data->ctx_lock);
	}
	klist_iter_exit(&device_iterator);

	if (!device_node) {
		/**
		 * No free device found.
		 * Since we allocated a device with down_interruptible, this
		 * should not be able to happen.
		 * Number of available devices, which are contained in
		 * device_allocation, is therefore decremented by not doing
		 * an up(device_allocation).
		 */
		return -EBUSY;
	}

	*device_data = local_device_data;

	return 0;
}
