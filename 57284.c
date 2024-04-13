static void vmw_hw_surface_destroy(struct vmw_resource *res)
{

	struct vmw_private *dev_priv = res->dev_priv;
	struct vmw_surface *srf;
	void *cmd;

	if (res->func->destroy == vmw_gb_surface_destroy) {
		(void) vmw_gb_surface_destroy(res);
		return;
	}

	if (res->id != -1) {

		cmd = vmw_fifo_reserve(dev_priv, vmw_surface_destroy_size());
		if (unlikely(!cmd)) {
			DRM_ERROR("Failed reserving FIFO space for surface "
				  "destruction.\n");
			return;
		}

		vmw_surface_destroy_encode(res->id, cmd);
		vmw_fifo_commit(dev_priv, vmw_surface_destroy_size());

		/*
		 * used_memory_size_atomic, or separate lock
		 * to avoid taking dev_priv::cmdbuf_mutex in
		 * the destroy path.
		 */

		mutex_lock(&dev_priv->cmdbuf_mutex);
		srf = vmw_res_to_srf(res);
		dev_priv->used_memory_size -= res->backup_size;
		mutex_unlock(&dev_priv->cmdbuf_mutex);
	}
	vmw_fifo_resource_dec(dev_priv);
}
