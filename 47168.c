static void oz_get_hub_descriptor(struct usb_hcd *hcd,
				struct usb_hub_descriptor *desc)
{
	memset(desc, 0, sizeof(*desc));
	desc->bDescriptorType = 0x29;
	desc->bDescLength = 9;
	desc->wHubCharacteristics = cpu_to_le16(0x0001);
	desc->bNbrPorts = OZ_NB_PORTS;
}
