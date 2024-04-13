static int catc_ctrl_msg(struct catc *catc, u8 dir, u8 request, u16 value, u16 index, void *buf, int len)
{
        int retval = usb_control_msg(catc->usbdev,
		dir ? usb_rcvctrlpipe(catc->usbdev, 0) : usb_sndctrlpipe(catc->usbdev, 0),
		 request, 0x40 | dir, value, index, buf, len, 1000);
        return retval < 0 ? retval : 0;
}
