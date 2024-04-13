static void usbip_pack_iso(struct usbip_iso_packet_descriptor *iso,
			   struct usb_iso_packet_descriptor *uiso, int pack)
{
	if (pack) {
		iso->offset		= uiso->offset;
		iso->length		= uiso->length;
		iso->status		= uiso->status;
		iso->actual_length	= uiso->actual_length;
	} else {
		uiso->offset		= iso->offset;
		uiso->length		= iso->length;
		uiso->status		= iso->status;
		uiso->actual_length	= iso->actual_length;
	}
}
