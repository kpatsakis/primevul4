static int read_ram(struct usb_device *dev, int start_address,
						int length, __u8 *buffer)
{
	return read_download_mem(dev, start_address, length,
					DTK_ADDR_SPACE_XDATA, buffer);
}
