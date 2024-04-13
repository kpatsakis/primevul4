static inline void endpoint_update(int edi,
				   struct usb_host_endpoint **in,
				   struct usb_host_endpoint **out,
				   struct usb_host_endpoint *e)
{
	if (edi) {
		if (!*in)
			*in = e;
	} else {
		if (!*out)
			*out = e;
	}
}
