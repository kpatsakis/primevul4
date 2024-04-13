static unsigned int tweak_transfer_flags(unsigned int flags)
{
	flags &= ~URB_NO_TRANSFER_DMA_MAP;
	return flags;
}
