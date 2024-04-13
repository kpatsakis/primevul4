fb_blank(struct fb_info *info, int blank)
{	
	struct fb_event event;
	int ret = -EINVAL, early_ret;

 	if (blank > FB_BLANK_POWERDOWN)
 		blank = FB_BLANK_POWERDOWN;

	event.info = info;
	event.data = &blank;

	early_ret = fb_notifier_call_chain(FB_EARLY_EVENT_BLANK, &event);

	if (info->fbops->fb_blank)
 		ret = info->fbops->fb_blank(blank, info);

	if (!ret)
		fb_notifier_call_chain(FB_EVENT_BLANK, &event);
	else {
		/*
		 * if fb_blank is failed then revert effects of
		 * the early blank event.
		 */
		if (!early_ret)
			fb_notifier_call_chain(FB_R_EARLY_EVENT_BLANK, &event);
	}

 	return ret;
}
