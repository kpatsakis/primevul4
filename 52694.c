static int visor_calc_num_ports(struct usb_serial *serial)
{
	int num_ports = (int)(long)(usb_get_serial_data(serial));

	if (num_ports)
		usb_set_serial_data(serial, NULL);

	return num_ports;
}
