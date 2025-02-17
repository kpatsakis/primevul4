vc4_reset_work(struct work_struct *work)
{
	struct vc4_dev *vc4 =
		container_of(work, struct vc4_dev, hangcheck.reset_work);

	vc4_save_hang_state(vc4->dev);

	vc4_reset(vc4->dev);
}
