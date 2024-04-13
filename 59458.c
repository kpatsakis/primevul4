vc4_submit_cl_ioctl(struct drm_device *dev, void *data,
		    struct drm_file *file_priv)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	struct drm_vc4_submit_cl *args = data;
	struct vc4_exec_info *exec;
	int ret = 0;

	if ((args->flags & ~VC4_SUBMIT_CL_USE_CLEAR_COLOR) != 0) {
		DRM_ERROR("Unknown flags: 0x%02x\n", args->flags);
		return -EINVAL;
	}

	exec = kcalloc(1, sizeof(*exec), GFP_KERNEL);
	if (!exec) {
		DRM_ERROR("malloc failure on exec struct\n");
		return -ENOMEM;
	}

	mutex_lock(&vc4->power_lock);
	if (vc4->power_refcount++ == 0)
		ret = pm_runtime_get_sync(&vc4->v3d->pdev->dev);
	mutex_unlock(&vc4->power_lock);
	if (ret < 0) {
		kfree(exec);
		return ret;
	}

	exec->args = args;
	INIT_LIST_HEAD(&exec->unref_list);

	ret = vc4_cl_lookup_bos(dev, file_priv, exec);
	if (ret)
		goto fail;

	if (exec->args->bin_cl_size != 0) {
		ret = vc4_get_bcl(dev, exec);
		if (ret)
			goto fail;
	} else {
		exec->ct0ca = 0;
		exec->ct0ea = 0;
	}

	ret = vc4_get_rcl(dev, exec);
	if (ret)
		goto fail;

	/* Clear this out of the struct we'll be putting in the queue,
	 * since it's part of our stack.
	 */
	exec->args = NULL;

	vc4_queue_submit(dev, exec);

	/* Return the seqno for our job. */
	args->seqno = vc4->emit_seqno;

	return 0;

fail:
	vc4_complete_exec(vc4->dev, exec);

	return ret;
}
