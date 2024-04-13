vc4_submit_next_render_job(struct drm_device *dev)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	struct vc4_exec_info *exec = vc4_first_render_job(vc4);

	if (!exec)
		return;

	submit_cl(dev, 1, exec->ct1ca, exec->ct1ea);
}
