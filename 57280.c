static int vmw_gb_surface_create(struct vmw_resource *res)
{
	struct vmw_private *dev_priv = res->dev_priv;
	struct vmw_surface *srf = vmw_res_to_srf(res);
	uint32_t cmd_len, cmd_id, submit_len;
	int ret;
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdDefineGBSurface body;
	} *cmd;
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdDefineGBSurface_v2 body;
	} *cmd2;

	if (likely(res->id != -1))
		return 0;

	vmw_fifo_resource_inc(dev_priv);
	ret = vmw_resource_alloc_id(res);
	if (unlikely(ret != 0)) {
		DRM_ERROR("Failed to allocate a surface id.\n");
		goto out_no_id;
	}

	if (unlikely(res->id >= VMWGFX_NUM_GB_SURFACE)) {
		ret = -EBUSY;
		goto out_no_fifo;
	}

	if (srf->array_size > 0) {
		/* has_dx checked on creation time. */
		cmd_id = SVGA_3D_CMD_DEFINE_GB_SURFACE_V2;
		cmd_len = sizeof(cmd2->body);
		submit_len = sizeof(*cmd2);
	} else {
		cmd_id = SVGA_3D_CMD_DEFINE_GB_SURFACE;
		cmd_len = sizeof(cmd->body);
		submit_len = sizeof(*cmd);
	}

	cmd = vmw_fifo_reserve(dev_priv, submit_len);
	cmd2 = (typeof(cmd2))cmd;
	if (unlikely(!cmd)) {
		DRM_ERROR("Failed reserving FIFO space for surface "
			  "creation.\n");
		ret = -ENOMEM;
		goto out_no_fifo;
	}

	if (srf->array_size > 0) {
		cmd2->header.id = cmd_id;
		cmd2->header.size = cmd_len;
		cmd2->body.sid = srf->res.id;
		cmd2->body.surfaceFlags = srf->flags;
		cmd2->body.format = cpu_to_le32(srf->format);
		cmd2->body.numMipLevels = srf->mip_levels[0];
		cmd2->body.multisampleCount = srf->multisample_count;
		cmd2->body.autogenFilter = srf->autogen_filter;
		cmd2->body.size.width = srf->base_size.width;
		cmd2->body.size.height = srf->base_size.height;
		cmd2->body.size.depth = srf->base_size.depth;
		cmd2->body.arraySize = srf->array_size;
	} else {
		cmd->header.id = cmd_id;
		cmd->header.size = cmd_len;
		cmd->body.sid = srf->res.id;
		cmd->body.surfaceFlags = srf->flags;
		cmd->body.format = cpu_to_le32(srf->format);
		cmd->body.numMipLevels = srf->mip_levels[0];
		cmd->body.multisampleCount = srf->multisample_count;
		cmd->body.autogenFilter = srf->autogen_filter;
		cmd->body.size.width = srf->base_size.width;
		cmd->body.size.height = srf->base_size.height;
		cmd->body.size.depth = srf->base_size.depth;
	}

	vmw_fifo_commit(dev_priv, submit_len);

	return 0;

out_no_fifo:
	vmw_resource_release_id(res);
out_no_id:
	vmw_fifo_resource_dec(dev_priv);
	return ret;
}
