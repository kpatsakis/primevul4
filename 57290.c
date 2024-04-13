int vmw_surface_define_ioctl(struct drm_device *dev, void *data,
			     struct drm_file *file_priv)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	struct vmw_user_surface *user_srf;
	struct vmw_surface *srf;
	struct vmw_resource *res;
	struct vmw_resource *tmp;
	union drm_vmw_surface_create_arg *arg =
	    (union drm_vmw_surface_create_arg *)data;
	struct drm_vmw_surface_create_req *req = &arg->req;
	struct drm_vmw_surface_arg *rep = &arg->rep;
	struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;
	int ret;
	int i, j;
	uint32_t cur_bo_offset;
	struct drm_vmw_size *cur_size;
	struct vmw_surface_offset *cur_offset;
	uint32_t num_sizes;
	uint32_t size;
	const struct svga3d_surface_desc *desc;

	if (unlikely(vmw_user_surface_size == 0))
		vmw_user_surface_size = ttm_round_pot(sizeof(*user_srf)) +
			128;

	num_sizes = 0;
	for (i = 0; i < DRM_VMW_MAX_SURFACE_FACES; ++i) {
		if (req->mip_levels[i] > DRM_VMW_MAX_MIP_LEVELS)
			return -EINVAL;
		num_sizes += req->mip_levels[i];
	}

	if (num_sizes > DRM_VMW_MAX_SURFACE_FACES * DRM_VMW_MAX_MIP_LEVELS ||
	    num_sizes == 0)
		return -EINVAL;

	size = vmw_user_surface_size + 128 +
		ttm_round_pot(num_sizes * sizeof(struct drm_vmw_size)) +
		ttm_round_pot(num_sizes * sizeof(struct vmw_surface_offset));


	desc = svga3dsurface_get_desc(req->format);
	if (unlikely(desc->block_desc == SVGA3DBLOCKDESC_NONE)) {
		DRM_ERROR("Invalid surface format for surface creation.\n");
		DRM_ERROR("Format requested is: %d\n", req->format);
		return -EINVAL;
	}

	ret = ttm_read_lock(&dev_priv->reservation_sem, true);
	if (unlikely(ret != 0))
		return ret;

	ret = ttm_mem_global_alloc(vmw_mem_glob(dev_priv),
				   size, false, true);
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

	srf = &user_srf->srf;
	res = &srf->res;

	srf->flags = req->flags;
	srf->format = req->format;
	srf->scanout = req->scanout;

	memcpy(srf->mip_levels, req->mip_levels, sizeof(srf->mip_levels));
	srf->num_sizes = num_sizes;
	user_srf->size = size;
	srf->sizes = memdup_user((struct drm_vmw_size __user *)(unsigned long)
				 req->size_addr,
				 sizeof(*srf->sizes) * srf->num_sizes);
	if (IS_ERR(srf->sizes)) {
		ret = PTR_ERR(srf->sizes);
		goto out_no_sizes;
	}
	srf->offsets = kmalloc_array(srf->num_sizes,
				     sizeof(*srf->offsets),
				     GFP_KERNEL);
	if (unlikely(!srf->offsets)) {
		ret = -ENOMEM;
		goto out_no_offsets;
	}

	srf->base_size = *srf->sizes;
	srf->autogen_filter = SVGA3D_TEX_FILTER_NONE;
	srf->multisample_count = 0;

	cur_bo_offset = 0;
	cur_offset = srf->offsets;
	cur_size = srf->sizes;

	for (i = 0; i < DRM_VMW_MAX_SURFACE_FACES; ++i) {
		for (j = 0; j < srf->mip_levels[i]; ++j) {
			uint32_t stride = svga3dsurface_calculate_pitch
				(desc, cur_size);

			cur_offset->face = i;
			cur_offset->mip = j;
			cur_offset->bo_offset = cur_bo_offset;
			cur_bo_offset += svga3dsurface_get_image_buffer_size
				(desc, cur_size, stride);
			++cur_offset;
			++cur_size;
		}
	}
	res->backup_size = cur_bo_offset;
	if (srf->scanout &&
	    srf->num_sizes == 1 &&
	    srf->sizes[0].width == 64 &&
	    srf->sizes[0].height == 64 &&
	    srf->format == SVGA3D_A8R8G8B8) {

		srf->snooper.image = kzalloc(64 * 64 * 4, GFP_KERNEL);
		if (!srf->snooper.image) {
			DRM_ERROR("Failed to allocate cursor_image\n");
			ret = -ENOMEM;
			goto out_no_copy;
		}
	} else {
		srf->snooper.image = NULL;
	}

	user_srf->prime.base.shareable = false;
	user_srf->prime.base.tfile = NULL;
	if (drm_is_primary_client(file_priv))
		user_srf->master = drm_master_get(file_priv->master);

	/**
	 * From this point, the generic resource management functions
	 * destroy the object on failure.
	 */

	ret = vmw_surface_init(dev_priv, srf, vmw_user_surface_free);
	if (unlikely(ret != 0))
		goto out_unlock;

	/*
	 * A gb-aware client referencing a shared surface will
	 * expect a backup buffer to be present.
	 */
	if (dev_priv->has_mob && req->shareable) {
		uint32_t backup_handle;

		ret = vmw_user_dmabuf_alloc(dev_priv, tfile,
					    res->backup_size,
					    true,
					    &backup_handle,
					    &res->backup,
					    &user_srf->backup_base);
		if (unlikely(ret != 0)) {
			vmw_resource_unreference(&res);
			goto out_unlock;
		}
	}

	tmp = vmw_resource_reference(&srf->res);
	ret = ttm_prime_object_init(tfile, res->backup_size, &user_srf->prime,
				    req->shareable, VMW_RES_SURFACE,
				    &vmw_user_surface_base_release, NULL);

	if (unlikely(ret != 0)) {
		vmw_resource_unreference(&tmp);
		vmw_resource_unreference(&res);
		goto out_unlock;
	}

	rep->sid = user_srf->prime.base.hash.key;
	vmw_resource_unreference(&res);

	ttm_read_unlock(&dev_priv->reservation_sem);
	return 0;
out_no_copy:
	kfree(srf->offsets);
out_no_offsets:
	kfree(srf->sizes);
out_no_sizes:
	ttm_prime_object_kfree(user_srf, prime);
out_no_user_srf:
	ttm_mem_global_free(vmw_mem_glob(dev_priv), size);
out_unlock:
	ttm_read_unlock(&dev_priv->reservation_sem);
	return ret;
}
