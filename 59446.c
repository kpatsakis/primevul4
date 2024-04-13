vc4_gem_destroy(struct drm_device *dev)
{
	struct vc4_dev *vc4 = to_vc4_dev(dev);

	/* Waiting for exec to finish would need to be done before
	 * unregistering V3D.
	 */
	WARN_ON(vc4->emit_seqno != vc4->finished_seqno);

	/* V3D should already have disabled its interrupt and cleared
	 * the overflow allocation registers.  Now free the object.
	 */
	if (vc4->overflow_mem) {
		drm_gem_object_unreference_unlocked(&vc4->overflow_mem->base.base);
		vc4->overflow_mem = NULL;
	}

	if (vc4->hang_state)
		vc4_free_hang_state(dev, vc4->hang_state);

	vc4_bo_cache_destroy(dev);
}
