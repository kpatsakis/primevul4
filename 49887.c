static void release_hash_device(struct hash_device_data *device_data)
{
	spin_lock(&device_data->ctx_lock);
	device_data->current_ctx->device = NULL;
	device_data->current_ctx = NULL;
	spin_unlock(&device_data->ctx_lock);

	/*
	 * The down_interruptible part for this semaphore is called in
	 * cryp_get_device_data.
	 */
	up(&driver_data.device_allocation);
}
