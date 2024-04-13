vc4_move_job_to_render(struct drm_device *dev, struct vc4_exec_info *exec)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);
	bool was_empty = list_empty(&vc4->render_job_list);

	list_move_tail(&exec->head, &vc4->render_job_list);
	if (was_empty)
		vc4_submit_next_render_job(dev);
}
