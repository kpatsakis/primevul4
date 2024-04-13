int vmw_surface_gb_priv_define(struct drm_device *dev,
			       uint32_t user_accounting_size,
			       uint32_t svga3d_flags,
			       SVGA3dSurfaceFormat format,
			       bool for_scanout,
			       uint32_t num_mip_levels,
			       uint32_t multisample_count,
			       uint32_t array_size,
			       struct drm_vmw_size size,
			       struct vmw_surface **srf_out)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct vmw_user_surface *user_srf;
	struct vmw_surface *srf;
	int ret;
	u32 num_layers;

	*srf_out = NULL;

	if (for_scanout) {
		uint32_t max_width, max_height;

		if (!svga3dsurface_is_screen_target_format(format)) {
			DRM_ERROR("Invalid Screen Target surface format.");
			return -EINVAL;
		}

		max_width = min(dev_priv->texture_max_width,
				dev_priv->stdu_max_width);
		max_height = min(dev_priv->texture_max_height,
				 dev_priv->stdu_max_height);

		if (size.width > max_width || size.height > max_height) {
			DRM_ERROR("%ux%u\n, exeeds max surface size %ux%u",
				  size.width, size.height,
				  max_width, max_height);
			return -EINVAL;
		}
	} else {
		const struct svga3d_surface_desc *desc;

		desc = svga3dsurface_get_desc(format);
		if (unlikely(desc->block_desc == SVGA3DBLOCKDESC_NONE)) {
			DRM_ERROR("Invalid surface format.\n");
			return -EINVAL;
		}
	}

	/* array_size must be null for non-GL3 host. */
	if (array_size > 0 && !dev_priv->has_dx) {
		DRM_ERROR("Tried to create DX surface on non-DX host.\n");
		return -EINVAL;
	}

	ret = ttm_read_lock(&dev_priv->reservation_sem, true);
	if (unlikely(ret != 0))
		return ret;

	ret = ttm_mem_global_alloc(vmw_mem_glob(dev_priv),
				   user_accounting_size, false, true);
	if (unlikely(ret != 0)) {
		if (ret != -ERESTARTSYS)
			DRM_ERROR("Out of graphics memory for surface"
				  " creation.\n");
		goto out_unlock;
	}

	user_srf = kzalloc(sizeof(*user_srf), GFP_KERNEL);
	if (unlikely(!user_srf)) {
		ret = -ENOMEM;
		goto out_no_user_srf;
	}

	*srf_out  = &user_srf->srf;
	user_srf->size = user_accounting_size;
	user_srf->prime.base.shareable = false;
	user_srf->prime.base.tfile     = NULL;

	srf = &user_srf->srf;
	srf->flags             = svga3d_flags;
	srf->format            = format;
	srf->scanout           = for_scanout;
	srf->mip_levels[0]     = num_mip_levels;
	srf->num_sizes         = 1;
	srf->sizes             = NULL;
	srf->offsets           = NULL;
	srf->base_size         = size;
	srf->autogen_filter    = SVGA3D_TEX_FILTER_NONE;
	srf->array_size        = array_size;
	srf->multisample_count = multisample_count;

	if (array_size)
		num_layers = array_size;
	else if (svga3d_flags & SVGA3D_SURFACE_CUBEMAP)
		num_layers = SVGA3D_MAX_SURFACE_FACES;
	else
		num_layers = 1;

	srf->res.backup_size   =
		svga3dsurface_get_serialized_size(srf->format,
						  srf->base_size,
						  srf->mip_levels[0],
						  num_layers);

	if (srf->flags & SVGA3D_SURFACE_BIND_STREAM_OUTPUT)
		srf->res.backup_size += sizeof(SVGA3dDXSOState);

	if (dev_priv->active_display_unit == vmw_du_screen_target &&
	    for_scanout)
		srf->flags |= SVGA3D_SURFACE_SCREENTARGET;

	/*
	 * From this point, the generic resource management functions
	 * destroy the object on failure.
	 */
	ret = vmw_surface_init(dev_priv, srf, vmw_user_surface_free);

	ttm_read_unlock(&dev_priv->reservation_sem);
	return ret;

out_no_user_srf:
	ttm_mem_global_free(vmw_mem_glob(dev_priv), user_accounting_size);

out_unlock:
	ttm_read_unlock(&dev_priv->reservation_sem);
	return ret;
}
