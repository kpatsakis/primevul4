static void correct_endian_cmd_submit(struct usbip_header_cmd_submit *pdu,
				      int send)
{
	if (send) {
		pdu->transfer_flags = cpu_to_be32(pdu->transfer_flags);

		cpu_to_be32s(&pdu->transfer_buffer_length);
		cpu_to_be32s(&pdu->start_frame);
		cpu_to_be32s(&pdu->number_of_packets);
		cpu_to_be32s(&pdu->interval);
	} else {
		pdu->transfer_flags = be32_to_cpu(pdu->transfer_flags);

		be32_to_cpus(&pdu->transfer_buffer_length);
		be32_to_cpus(&pdu->start_frame);
		be32_to_cpus(&pdu->number_of_packets);
		be32_to_cpus(&pdu->interval);
	}
}
