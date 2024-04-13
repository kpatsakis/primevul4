static int oz_get_port_status(struct usb_hcd *hcd, u16 windex, char *buf)
{
	struct oz_hcd *ozhcd;
	u32 status;

	if ((windex < 1) || (windex > OZ_NB_PORTS))
		return -EPIPE;
	ozhcd = oz_hcd_private(hcd);
	oz_dbg(HUB, "GetPortStatus windex = %d\n", windex);
	status = ozhcd->ports[windex-1].status;
	put_unaligned(cpu_to_le32(status), (__le32 *)buf);
	oz_dbg(HUB, "Port[%d] status = %x\n", windex, status);
	return 0;
}
