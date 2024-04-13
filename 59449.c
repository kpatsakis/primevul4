vc4_job_done_work(struct work_struct *work)
{
	struct vc4_dev *vc4 =
		container_of(work, struct vc4_dev, job_done_work);

	vc4_job_handle_completed(vc4);
}
