static inline bool is_frwd(struct usb_device *dev)
{
	return ((le16_to_cpu(dev->descriptor.idVendor) == VENDOR_ID_FRWD) &&
		(le16_to_cpu(dev->descriptor.idProduct) == PRODUCT_ID_CYPHIDCOM_FRWD));
}
