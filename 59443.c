vc4_cl_lookup_bos(struct drm_device *dev,
		  struct drm_file *file_priv,
		  struct vc4_exec_info *exec)
{
	struct drm_vc4_submit_cl *args = exec->args;
	uint32_t *handles;
	int ret = 0;
	int i;

	exec->bo_count = args->bo_handle_count;

	if (!exec->bo_count) {
		/* See comment on bo_index for why we have to check
		 * this.
		 */
		DRM_ERROR("Rendering requires BOs to validate\n");
		return -EINVAL;
	}

	exec->bo = drm_calloc_large(exec->bo_count,
				    sizeof(struct drm_gem_cma_object *));
	if (!exec->bo) {
		DRM_ERROR("Failed to allocate validated BO pointers\n");
		return -ENOMEM;
	}

	handles = drm_malloc_ab(exec->bo_count, sizeof(uint32_t));
	if (!handles) {
		ret = -ENOMEM;
		DRM_ERROR("Failed to allocate incoming GEM handles\n");
		goto fail;
	}

	if (copy_from_user(handles,
			   (void __user *)(uintptr_t)args->bo_handles,
			   exec->bo_count * sizeof(uint32_t))) {
		ret = -EFAULT;
		DRM_ERROR("Failed to copy in GEM handles\n");
		goto fail;
	}

	spin_lock(&file_priv->table_lock);
	for (i = 0; i < exec->bo_count; i++) {
		struct drm_gem_object *bo = idr_find(&file_priv->object_idr,
						     handles[i]);
		if (!bo) {
			DRM_ERROR("Failed to look up GEM BO %d: %d\n",
				  i, handles[i]);
			ret = -EINVAL;
			spin_unlock(&file_priv->table_lock);
			goto fail;
		}
		drm_gem_object_reference(bo);
		exec->bo[i] = (struct drm_gem_cma_object *)bo;
	}
	spin_unlock(&file_priv->table_lock);

fail:
	drm_free_large(handles);
	return ret;
}
