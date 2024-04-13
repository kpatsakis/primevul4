static int __init cryptd_init(void)
{
	int err;

	err = cryptd_init_queue(&queue, CRYPTD_MAX_CPU_QLEN);
	if (err)
		return err;

	err = crypto_register_template(&cryptd_tmpl);
	if (err)
		cryptd_fini_queue(&queue);

	return err;
}
