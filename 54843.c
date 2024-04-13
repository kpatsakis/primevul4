static int usb_audio_resume(struct usb_interface *intf)
{
	return __usb_audio_resume(intf, false);
}
