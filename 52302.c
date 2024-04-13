static void usbip_dump_pipe(unsigned int p)
{
	unsigned char type = usb_pipetype(p);
	unsigned char ep   = usb_pipeendpoint(p);
	unsigned char dev  = usb_pipedevice(p);
	unsigned char dir  = usb_pipein(p);

	pr_debug("dev(%d) ep(%d) [%s] ", dev, ep, dir ? "IN" : "OUT");

	switch (type) {
	case PIPE_ISOCHRONOUS:
		pr_debug("ISO\n");
		break;
	case PIPE_INTERRUPT:
		pr_debug("INT\n");
		break;
	case PIPE_CONTROL:
		pr_debug("CTRL\n");
		break;
	case PIPE_BULK:
		pr_debug("BULK\n");
		break;
	default:
		pr_debug("ERR\n");
		break;
	}
}
