vc4_free_hang_state(struct drm_device *dev, struct vc4_hang_state *state)
{
	unsigned int i;

	for (i = 0; i < state->user_state.bo_count; i++)
		drm_gem_object_unreference_unlocked(state->bo[i]);

	kfree(state);
}
