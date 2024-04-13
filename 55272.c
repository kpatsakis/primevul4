static inline void lego_usb_tower_debug_data(struct device *dev,
					     const char *function, int size,
					     const unsigned char *data)
{
	dev_dbg(dev, "%s - length = %d, data = %*ph\n",
		function, size, size, data);
}
