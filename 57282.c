int vmw_gb_surface_reference_ioctl(struct drm_device *dev, void *data,
				   struct drm_file *file_priv)
{
	struct vmw_private *dev_priv = vmw_priv(dev);
	union drm_vmw_gb_surface_reference_arg *arg =
	    (union drm_vmw_gb_surface_reference_arg *)data;
	struct drm_vmw_surface_arg *req = &arg->req;
	struct drm_vmw_gb_surface_ref_rep *rep = &arg->rep;
	struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;
	struct vmw_surface *srf;
	struct vmw_user_surface *user_srf;
	struct ttm_base_object *base;
	uint32_t backup_handle;
	int ret = -EINVAL;

	ret = vmw_surface_handle_reference(dev_priv, file_priv, req->sid,
					   req->handle_type, &base);
	if (unlikely(ret != 0))
		return ret;

	user_srf = container_of(base, struct vmw_user_surface, prime.base);
	srf = &user_srf->srf;
	if (!srf->res.backup) {
		DRM_ERROR("Shared GB surface is missing a backup buffer.\n");
		goto out_bad_resource;
	}

	mutex_lock(&dev_priv->cmdbuf_mutex); /* Protect res->backup */
	ret = vmw_user_dmabuf_reference(tfile, srf->res.backup,
					&backup_handle);
	mutex_unlock(&dev_priv->cmdbuf_mutex);

	if (unlikely(ret != 0)) {
		DRM_ERROR("Could not add a reference to a GB surface "
			  "backup buffer.\n");
		(void) ttm_ref_object_base_unref(tfile, base->hash.key,
						 TTM_REF_USAGE);
		goto out_bad_resource;
	}

	rep->creq.svga3d_flags = srf->flags;
	rep->creq.format = srf->format;
	rep->creq.mip_levels = srf->mip_levels[0];
	rep->creq.drm_surface_flags = 0;
	rep->creq.multisample_count = srf->multisample_count;
	rep->creq.autogen_filter = srf->autogen_filter;
	rep->creq.array_size = srf->array_size;
	rep->creq.buffer_handle = backup_handle;
	rep->creq.base_size = srf->base_size;
	rep->crep.handle = user_srf->prime.base.hash.key;
	rep->crep.backup_size = srf->res.backup_size;
	rep->crep.buffer_handle = backup_handle;
	rep->crep.buffer_map_handle =
		drm_vma_node_offset_addr(&srf->res.backup->base.vma_node);
	rep->crep.buffer_size = srf->res.backup->base.num_pages * PAGE_SIZE;

out_bad_resource:
	ttm_base_object_unref(&base);

	return ret;
}
