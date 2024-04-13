static void vmw_user_surface_free(struct vmw_resource *res)
{
	struct vmw_surface *srf = vmw_res_to_srf(res);
	struct vmw_user_surface *user_srf =
	    container_of(srf, struct vmw_user_surface, srf);
	struct vmw_private *dev_priv = srf->res.dev_priv;
	uint32_t size = user_srf->size;

	if (user_srf->master)
		drm_master_put(&user_srf->master);
	kfree(srf->offsets);
	kfree(srf->sizes);
	kfree(srf->snooper.image);
	ttm_prime_object_kfree(user_srf, prime);
	ttm_mem_global_free(vmw_mem_glob(dev_priv), size);
}
