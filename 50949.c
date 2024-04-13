static int __ffs_ep0_stall(struct ffs_data *ffs)
{
	if (ffs->ev.can_stall) {
		pr_vdebug("ep0 stall\n");
		usb_ep_set_halt(ffs->gadget->ep0);
		ffs->setup_state = FFS_NO_SETUP;
		return -EL2HLT;
	} else {
		pr_debug("bogus ep0 stall!\n");
		return -ESRCH;
	}
}
