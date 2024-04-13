vmw_surface_handle_reference(struct vmw_private *dev_priv,
			     struct drm_file *file_priv,
			     uint32_t u_handle,
			     enum drm_vmw_handle_type handle_type,
			     struct ttm_base_object **base_p)
{
	struct ttm_object_file *tfile = vmw_fpriv(file_priv)->tfile;
	struct vmw_user_surface *user_srf;
	uint32_t handle;
	struct ttm_base_object *base;
	int ret;
	bool require_exist = false;

	if (handle_type == DRM_VMW_HANDLE_PRIME) {
		ret = ttm_prime_fd_to_handle(tfile, u_handle, &handle);
		if (unlikely(ret != 0))
			return ret;
	} else {
		if (unlikely(drm_is_render_client(file_priv)))
			require_exist = true;

		if (ACCESS_ONCE(vmw_fpriv(file_priv)->locked_master)) {
			DRM_ERROR("Locked master refused legacy "
				  "surface reference.\n");
			return -EACCES;
		}

		handle = u_handle;
	}

	ret = -EINVAL;
	base = ttm_base_object_lookup_for_ref(dev_priv->tdev, handle);
	if (unlikely(!base)) {
		DRM_ERROR("Could not find surface to reference.\n");
		goto out_no_lookup;
	}

	if (unlikely(ttm_base_object_type(base) != VMW_RES_SURFACE)) {
		DRM_ERROR("Referenced object is not a surface.\n");
		goto out_bad_resource;
	}

	if (handle_type != DRM_VMW_HANDLE_PRIME) {
		user_srf = container_of(base, struct vmw_user_surface,
					prime.base);

		/*
		 * Make sure the surface creator has the same
		 * authenticating master, or is already registered with us.
		 */
		if (drm_is_primary_client(file_priv) &&
		    user_srf->master != file_priv->master)
			require_exist = true;

		ret = ttm_ref_object_add(tfile, base, TTM_REF_USAGE, NULL,
					 require_exist);
		if (unlikely(ret != 0)) {
			DRM_ERROR("Could not add a reference to a surface.\n");
			goto out_bad_resource;
		}
	}

	*base_p = base;
	return 0;

out_bad_resource:
	ttm_base_object_unref(&base);
out_no_lookup:
	if (handle_type == DRM_VMW_HANDLE_PRIME)
		(void) ttm_ref_object_base_unref(tfile, handle, TTM_REF_USAGE);

	return ret;
}
