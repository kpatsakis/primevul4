static int tx_active(struct edgeport_port *port)
{
	int status;
	struct out_endpoint_desc_block *oedb;
	__u8 *lsr;
	int bytes_left = 0;

	oedb = kmalloc(sizeof(*oedb), GFP_KERNEL);
	if (!oedb) {
		dev_err(&port->port->dev, "%s - out of memory\n", __func__);
		return -ENOMEM;
	}

	lsr = kmalloc(1, GFP_KERNEL);	/* Sigh, that's right, just one byte,
					   as not all platforms can do DMA
					   from stack */
	if (!lsr) {
		kfree(oedb);
		return -ENOMEM;
	}
	/* Read the DMA Count Registers */
	status = read_ram(port->port->serial->dev, port->dma_address,
						sizeof(*oedb), (void *)oedb);
	if (status)
		goto exit_is_tx_active;

	dev_dbg(&port->port->dev, "%s - XByteCount    0x%X\n", __func__, oedb->XByteCount);

	/* and the LSR */
	status = read_ram(port->port->serial->dev,
			port->uart_base + UMPMEM_OFFS_UART_LSR, 1, lsr);

	if (status)
		goto exit_is_tx_active;
	dev_dbg(&port->port->dev, "%s - LSR = 0x%X\n", __func__, *lsr);

	/* If either buffer has data or we are transmitting then return TRUE */
	if ((oedb->XByteCount & 0x80) != 0)
		bytes_left += 64;

	if ((*lsr & UMP_UART_LSR_TX_MASK) == 0)
		bytes_left += 1;

	/* We return Not Active if we get any kind of error */
exit_is_tx_active:
	dev_dbg(&port->port->dev, "%s - return %d\n", __func__, bytes_left);

	kfree(lsr);
	kfree(oedb);
	return bytes_left;
}
