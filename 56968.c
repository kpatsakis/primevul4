static void xen_vbd_resize(struct xen_blkif *blkif)
{
	struct xen_vbd *vbd = &blkif->vbd;
	struct xenbus_transaction xbt;
	int err;
	struct xenbus_device *dev = xen_blkbk_xenbus(blkif->be);
	unsigned long long new_size = vbd_sz(vbd);

	pr_info("VBD Resize: Domid: %d, Device: (%d, %d)\n",
		blkif->domid, MAJOR(vbd->pdevice), MINOR(vbd->pdevice));
	pr_info("VBD Resize: new size %llu\n", new_size);
	vbd->size = new_size;
again:
	err = xenbus_transaction_start(&xbt);
	if (err) {
		pr_warn("Error starting transaction\n");
		return;
	}
	err = xenbus_printf(xbt, dev->nodename, "sectors", "%llu",
			    (unsigned long long)vbd_sz(vbd));
	if (err) {
		pr_warn("Error writing new size\n");
		goto abort;
	}
	/*
	 * Write the current state; we will use this to synchronize
	 * the front-end. If the current state is "connected" the
	 * front-end will get the new size information online.
	 */
	err = xenbus_printf(xbt, dev->nodename, "state", "%d", dev->state);
	if (err) {
		pr_warn("Error writing the state\n");
		goto abort;
	}

	err = xenbus_transaction_end(xbt, 0);
	if (err == -EAGAIN)
		goto again;
	if (err)
		pr_warn("Error ending transaction\n");
	return;
abort:
	xenbus_transaction_end(xbt, 1);
}
