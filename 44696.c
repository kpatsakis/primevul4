static __init void floppy_async_init(void *data, async_cookie_t cookie)
{
	do_floppy_init();
}
