static void correct_endian_ret_submit(struct usbip_header_ret_submit *pdu,
				      int send)
{
	if (send) {
		cpu_to_be32s(&pdu->status);
		cpu_to_be32s(&pdu->actual_length);
		cpu_to_be32s(&pdu->start_frame);
		cpu_to_be32s(&pdu->number_of_packets);
		cpu_to_be32s(&pdu->error_count);
	} else {
		be32_to_cpus(&pdu->status);
		be32_to_cpus(&pdu->actual_length);
		be32_to_cpus(&pdu->start_frame);
		be32_to_cpus(&pdu->number_of_packets);
		be32_to_cpus(&pdu->error_count);
	}
}
