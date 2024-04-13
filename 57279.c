static int vmw_gb_surface_bind(struct vmw_resource *res,
			       struct ttm_validate_buffer *val_buf)
{
	struct vmw_private *dev_priv = res->dev_priv;
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdBindGBSurface body;
	} *cmd1;
	struct {
		SVGA3dCmdHeader header;
		SVGA3dCmdUpdateGBSurface body;
	} *cmd2;
	uint32_t submit_size;
	struct ttm_buffer_object *bo = val_buf->bo;

	BUG_ON(bo->mem.mem_type != VMW_PL_MOB);

	submit_size = sizeof(*cmd1) + (res->backup_dirty ? sizeof(*cmd2) : 0);

	cmd1 = vmw_fifo_reserve(dev_priv, submit_size);
	if (unlikely(!cmd1)) {
		DRM_ERROR("Failed reserving FIFO space for surface "
			  "binding.\n");
		return -ENOMEM;
	}

	cmd1->header.id = SVGA_3D_CMD_BIND_GB_SURFACE;
	cmd1->header.size = sizeof(cmd1->body);
	cmd1->body.sid = res->id;
	cmd1->body.mobid = bo->mem.start;
	if (res->backup_dirty) {
		cmd2 = (void *) &cmd1[1];
		cmd2->header.id = SVGA_3D_CMD_UPDATE_GB_SURFACE;
		cmd2->header.size = sizeof(cmd2->body);
		cmd2->body.sid = res->id;
		res->backup_dirty = false;
	}
	vmw_fifo_commit(dev_priv, submit_size);

	return 0;
}
