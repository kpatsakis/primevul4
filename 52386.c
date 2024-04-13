static ssize_t ims_pcu_attribute_show(struct device *dev,
				      struct device_attribute *dattr,
				      char *buf)
{
	struct usb_interface *intf = to_usb_interface(dev);
	struct ims_pcu *pcu = usb_get_intfdata(intf);
	struct ims_pcu_attribute *attr =
			container_of(dattr, struct ims_pcu_attribute, dattr);
	char *field = (char *)pcu + attr->field_offset;

	return scnprintf(buf, PAGE_SIZE, "%.*s\n", attr->field_length, field);
}
