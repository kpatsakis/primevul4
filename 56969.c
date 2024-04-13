static void virtio_gpu_init_ttm_placement(struct virtio_gpu_object *vgbo,
					  bool pinned)
{
	u32 c = 1;
	u32 pflag = pinned ? TTM_PL_FLAG_NO_EVICT : 0;

	vgbo->placement.placement = &vgbo->placement_code;
	vgbo->placement.busy_placement = &vgbo->placement_code;
	vgbo->placement_code.fpfn = 0;
	vgbo->placement_code.lpfn = 0;
	vgbo->placement_code.flags =
		TTM_PL_MASK_CACHING | TTM_PL_FLAG_TT | pflag;
	vgbo->placement.num_placement = c;
	vgbo->placement.num_busy_placement = c;

}
