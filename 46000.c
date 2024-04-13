void do_become_nonbusy(struct comedi_device *dev, struct comedi_subdevice *s)
{
	struct comedi_async *async = s->async;

	comedi_set_subdevice_runflags(s, SRF_RUNNING, 0);
	if (async) {
		comedi_reset_async_buf(async);
		async->inttrig = NULL;
	} else {
		printk(KERN_ERR
		       "BUG: (?) do_become_nonbusy called with async=0\n");
	}

	s->busy = NULL;
}
