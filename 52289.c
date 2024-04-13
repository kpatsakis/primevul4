static void correct_endian_basic(struct usbip_header_basic *base, int send)
{
	if (send) {
		base->command	= cpu_to_be32(base->command);
		base->seqnum	= cpu_to_be32(base->seqnum);
		base->devid	= cpu_to_be32(base->devid);
		base->direction	= cpu_to_be32(base->direction);
		base->ep	= cpu_to_be32(base->ep);
	} else {
		base->command	= be32_to_cpu(base->command);
		base->seqnum	= be32_to_cpu(base->seqnum);
		base->devid	= be32_to_cpu(base->devid);
		base->direction	= be32_to_cpu(base->direction);
		base->ep	= be32_to_cpu(base->ep);
	}
}
