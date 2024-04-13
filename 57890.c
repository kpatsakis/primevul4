static void saa7164_bus_dumpmsg(struct saa7164_dev *dev, struct tmComResInfo *m,
				void *buf)
{
	dprintk(DBGLVL_BUS, "Dumping msg structure:\n");
	dprintk(DBGLVL_BUS, " .id               = %d\n",   m->id);
	dprintk(DBGLVL_BUS, " .flags            = 0x%x\n", m->flags);
	dprintk(DBGLVL_BUS, " .size             = 0x%x\n", m->size);
	dprintk(DBGLVL_BUS, " .command          = 0x%x\n", m->command);
	dprintk(DBGLVL_BUS, " .controlselector  = 0x%x\n", m->controlselector);
	dprintk(DBGLVL_BUS, " .seqno            = %d\n",   m->seqno);
	if (buf)
		dprintk(DBGLVL_BUS, " .buffer (ignored)\n");
}
