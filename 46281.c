int vivid_fb_init(struct vivid_dev *dev)
{
	int ret;

	dev->video_buffer_size = MAX_OSD_HEIGHT * MAX_OSD_WIDTH * 2;
	dev->video_vbase = kzalloc(dev->video_buffer_size, GFP_KERNEL | GFP_DMA32);
	if (dev->video_vbase == NULL)
		return -ENOMEM;
	dev->video_pbase = virt_to_phys(dev->video_vbase);

	pr_info("Framebuffer at 0x%lx, mapped to 0x%p, size %dk\n",
			dev->video_pbase, dev->video_vbase,
			dev->video_buffer_size / 1024);

	/* Set the startup video mode information */
	ret = vivid_fb_init_vidmode(dev);
	if (ret) {
		vivid_fb_release_buffers(dev);
		return ret;
	}

	vivid_clear_fb(dev);

	/* Register the framebuffer */
	if (register_framebuffer(&dev->fb_info) < 0) {
		vivid_fb_release_buffers(dev);
		return -EINVAL;
	}

	/* Set the card to the requested mode */
	vivid_fb_set_par(&dev->fb_info);
	return 0;

}
