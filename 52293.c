static void correct_endian_ret_unlink(struct usbip_header_ret_unlink *pdu,
				      int send)
{
	if (send)
		cpu_to_be32s(&pdu->status);
	else
		be32_to_cpus(&pdu->status);
}
