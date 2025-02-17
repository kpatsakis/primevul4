static void cleanup(struct wdm_device *desc)
{
	kfree(desc->sbuf);
	kfree(desc->inbuf);
	kfree(desc->orq);
	kfree(desc->irq);
	kfree(desc->ubuf);
	free_urbs(desc);
	kfree(desc);
}
