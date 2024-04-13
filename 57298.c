static int vmw_surface_init(struct vmw_private *dev_priv,
			    struct vmw_surface *srf,
			    void (*res_free) (struct vmw_resource *res))
{
	int ret;
	struct vmw_resource *res = &srf->res;

	BUG_ON(!res_free);
	if (!dev_priv->has_mob)
		vmw_fifo_resource_inc(dev_priv);
	ret = vmw_resource_init(dev_priv, res, true, res_free,
				(dev_priv->has_mob) ? &vmw_gb_surface_func :
				&vmw_legacy_surface_func);

	if (unlikely(ret != 0)) {
		if (!dev_priv->has_mob)
			vmw_fifo_resource_dec(dev_priv);
		res_free(res);
		return ret;
	}

	/*
	 * The surface won't be visible to hardware until a
	 * surface validate.
	 */

	INIT_LIST_HEAD(&srf->view_list);
	vmw_resource_activate(res, vmw_hw_surface_destroy);
	return ret;
}
