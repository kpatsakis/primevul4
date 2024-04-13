static void saa7164_bus_verify(struct saa7164_dev *dev)
{
	struct tmComResBusInfo *b = &dev->bus;
	int bug = 0;

	if (saa7164_readl(b->m_dwSetReadPos) > b->m_dwSizeSetRing)
		bug++;

	if (saa7164_readl(b->m_dwSetWritePos) > b->m_dwSizeSetRing)
		bug++;

	if (saa7164_readl(b->m_dwGetReadPos) > b->m_dwSizeGetRing)
		bug++;

	if (saa7164_readl(b->m_dwGetWritePos) > b->m_dwSizeGetRing)
		bug++;

	if (bug) {
		saa_debug = 0xffff; /* Ensure we get the bus dump */
		saa7164_bus_dump(dev);
		saa_debug = 1024; /* Ensure we get the bus dump */
		BUG();
	}
}
