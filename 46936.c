static void vmx_set_rvi(int vector)
{
	u16 status;
	u8 old;

	if (vector == -1)
		vector = 0;

	status = vmcs_read16(GUEST_INTR_STATUS);
	old = (u8)status & 0xff;
	if ((u8)vector != old) {
		status &= ~0xff;
		status |= (u8)vector;
		vmcs_write16(GUEST_INTR_STATUS, status);
	}
}
