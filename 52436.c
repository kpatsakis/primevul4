static void digi_close(struct usb_serial_port *port)
{
	DEFINE_WAIT(wait);
	int ret;
	unsigned char buf[32];
	struct digi_port *priv = usb_get_serial_port_data(port);

	mutex_lock(&port->serial->disc_mutex);
	/* if disconnected, just clear flags */
	if (port->serial->disconnected)
		goto exit;

	/* FIXME: Transmit idle belongs in the wait_unti_sent path */
	digi_transmit_idle(port, DIGI_CLOSE_TIMEOUT);

	/* disable input flow control */
	buf[0] = DIGI_CMD_SET_INPUT_FLOW_CONTROL;
	buf[1] = priv->dp_port_num;
	buf[2] = DIGI_DISABLE;
	buf[3] = 0;

	/* disable output flow control */
	buf[4] = DIGI_CMD_SET_OUTPUT_FLOW_CONTROL;
	buf[5] = priv->dp_port_num;
	buf[6] = DIGI_DISABLE;
	buf[7] = 0;

	/* disable reading modem signals automatically */
	buf[8] = DIGI_CMD_READ_INPUT_SIGNALS;
	buf[9] = priv->dp_port_num;
	buf[10] = DIGI_DISABLE;
	buf[11] = 0;

	/* disable receive */
	buf[12] = DIGI_CMD_RECEIVE_ENABLE;
	buf[13] = priv->dp_port_num;
	buf[14] = DIGI_DISABLE;
	buf[15] = 0;

	/* flush fifos */
	buf[16] = DIGI_CMD_IFLUSH_FIFO;
	buf[17] = priv->dp_port_num;
	buf[18] = DIGI_FLUSH_TX | DIGI_FLUSH_RX;
	buf[19] = 0;

	ret = digi_write_oob_command(port, buf, 20, 0);
	if (ret != 0)
		dev_dbg(&port->dev, "digi_close: write oob failed, ret=%d\n",
									ret);
	/* wait for final commands on oob port to complete */
	prepare_to_wait(&priv->dp_flush_wait, &wait,
			TASK_INTERRUPTIBLE);
	schedule_timeout(DIGI_CLOSE_TIMEOUT);
	finish_wait(&priv->dp_flush_wait, &wait);

	/* shutdown any outstanding bulk writes */
	usb_kill_urb(port->write_urb);
exit:
	spin_lock_irq(&priv->dp_port_lock);
	priv->dp_write_urb_in_use = 0;
	wake_up_interruptible(&priv->dp_close_wait);
	spin_unlock_irq(&priv->dp_port_lock);
	mutex_unlock(&port->serial->disc_mutex);
}
