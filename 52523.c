static void mct_u232_msr_to_state(struct usb_serial_port *port,
				  unsigned int *control_state, unsigned char msr)
{
	/* Translate Control Line states */
	if (msr & MCT_U232_MSR_DSR)
		*control_state |=  TIOCM_DSR;
	else
		*control_state &= ~TIOCM_DSR;
	if (msr & MCT_U232_MSR_CTS)
		*control_state |=  TIOCM_CTS;
	else
		*control_state &= ~TIOCM_CTS;
	if (msr & MCT_U232_MSR_RI)
		*control_state |=  TIOCM_RI;
	else
		*control_state &= ~TIOCM_RI;
	if (msr & MCT_U232_MSR_CD)
		*control_state |=  TIOCM_CD;
	else
		*control_state &= ~TIOCM_CD;
	dev_dbg(&port->dev, "msr_to_state: msr=0x%x ==> state=0x%x\n", msr, *control_state);
} /* mct_u232_msr_to_state */
