vc4_reset(struct drm_device *dev)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);

	DRM_INFO("Resetting GPU.\n");

	mutex_lock(&vc4->power_lock);
	if (vc4->power_refcount) {
		/* Power the device off and back on the by dropping the
		 * reference on runtime PM.
		 */
		pm_runtime_put_sync_suspend(&vc4->v3d->pdev->dev);
		pm_runtime_get_sync(&vc4->v3d->pdev->dev);
	}
	mutex_unlock(&vc4->power_lock);

	vc4_irq_reset(dev);

	/* Rearm the hangcheck -- another job might have been waiting
	 * for our hung one to get kicked off, and vc4_irq_reset()
	 * would have started it.
	 */
	vc4_queue_hangcheck(dev);
}
