static int vmw_gb_surface_destroy(struct vmw_resource *res)
{
	struct vmw_private *dev_priv = res->dev_priv;
	struct vmw_surface *srf = vmw_res_to_srf(res);
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdDestroyGBSurface body;
	} *cmd;

	if (likely(res->id == -1))
		return 0;

	mutex_lock(&dev_priv->binding_mutex);
	vmw_view_surface_list_destroy(dev_priv, &srf->view_list);
	vmw_binding_res_list_scrub(&res->binding_head);

	cmd = vmw_fifo_reserve(dev_priv, sizeof(*cmd));
	if (unlikely(!cmd)) {
		DRM_ERROR("Failed reserving FIFO space for surface "
			  "destruction.\n");
		mutex_unlock(&dev_priv->binding_mutex);
		return -ENOMEM;
	}

	cmd->header.id = SVGA_3D_CMD_DESTROY_GB_SURFACE;
	cmd->header.size = sizeof(cmd->body);
	cmd->body.sid = res->id;
	vmw_fifo_commit(dev_priv, sizeof(*cmd));
	mutex_unlock(&dev_priv->binding_mutex);
	vmw_resource_release_id(res);
	vmw_fifo_resource_dec(dev_priv);

	return 0;
}
