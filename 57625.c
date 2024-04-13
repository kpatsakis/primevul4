static void default_resize_settings(struct iw_resize_settings *rs)
{
	int i;
	rs->family = IW_RESIZETYPE_AUTO;
	rs->edge_policy = IW_EDGE_POLICY_STANDARD;
	rs->blur_factor = 1.0;
	rs->translate = 0.0;
	for(i=0;i<3;i++) {
		rs->channel_offset[i] = 0.0;
	}
}
