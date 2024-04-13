static void do_remove_conflicting_framebuffers(struct apertures_struct *a,
				     const char *name, bool primary)
{
	int i;

	/* check all firmware fbs and kick off if the base addr overlaps */
	for (i = 0 ; i < FB_MAX; i++) {
		struct apertures_struct *gen_aper;
		if (!registered_fb[i])
			continue;

		if (!(registered_fb[i]->flags & FBINFO_MISC_FIRMWARE))
			continue;

		gen_aper = registered_fb[i]->apertures;
		if (fb_do_apertures_overlap(gen_aper, a) ||
			(primary && gen_aper && gen_aper->count &&
			 gen_aper->ranges[0].base == VGA_FB_PHYS)) {

			printk(KERN_INFO "fb: conflicting fb hw usage "
			       "%s vs %s - removing generic driver\n",
			       name, registered_fb[i]->fix.id);
			do_unregister_framebuffer(registered_fb[i]);
		}
	}
}
