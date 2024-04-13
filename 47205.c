static inline int oz_usb_get_frame_number(void)
{
	return atomic_inc_return(&g_usb_frame_number);
}
