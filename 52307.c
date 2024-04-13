static void usbip_iso_packet_correct_endian(
		struct usbip_iso_packet_descriptor *iso, int send)
{
	/* does not need all members. but copy all simply. */
	if (send) {
		iso->offset	= cpu_to_be32(iso->offset);
		iso->length	= cpu_to_be32(iso->length);
		iso->status	= cpu_to_be32(iso->status);
		iso->actual_length = cpu_to_be32(iso->actual_length);
	} else {
		iso->offset	= be32_to_cpu(iso->offset);
		iso->length	= be32_to_cpu(iso->length);
		iso->status	= be32_to_cpu(iso->status);
		iso->actual_length = be32_to_cpu(iso->actual_length);
	}
}
