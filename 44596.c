static int __init aurule_init(void)
{
	int err;

	err = avc_add_callback(aurule_avc_callback, AVC_CALLBACK_RESET);
	if (err)
		panic("avc_add_callback() failed, error %d\n", err);

	return err;
}
