int saa7164_bus_setup(struct saa7164_dev *dev)
{
	struct tmComResBusInfo *b	= &dev->bus;

	mutex_init(&b->lock);

	b->Type			= TYPE_BUS_PCIe;
	b->m_wMaxReqSize	= SAA_DEVICE_MAXREQUESTSIZE;

	b->m_pdwSetRing		= (u8 __iomem *)(dev->bmmio +
		((u32)dev->busdesc.CommandRing));

	b->m_dwSizeSetRing	= SAA_DEVICE_BUFFERBLOCKSIZE;

	b->m_pdwGetRing		= (u8 __iomem *)(dev->bmmio +
		((u32)dev->busdesc.ResponseRing));

	b->m_dwSizeGetRing	= SAA_DEVICE_BUFFERBLOCKSIZE;

	b->m_dwSetWritePos	= ((u32)dev->intfdesc.BARLocation) +
		(2 * sizeof(u64));
	b->m_dwSetReadPos	= b->m_dwSetWritePos + (1 * sizeof(u32));

	b->m_dwGetWritePos	= b->m_dwSetWritePos + (2 * sizeof(u32));
	b->m_dwGetReadPos	= b->m_dwSetWritePos + (3 * sizeof(u32));

	return 0;
}
