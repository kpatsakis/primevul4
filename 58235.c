int dvb_usb_get_hexline(const struct firmware *fw, struct hexline *hx,
			       int *pos)
{
	u8 *b = (u8 *) &fw->data[*pos];
	int data_offs = 4;
	if (*pos >= fw->size)
		return 0;

	memset(hx,0,sizeof(struct hexline));

	hx->len  = b[0];

	if ((*pos + hx->len + 4) >= fw->size)
		return -EINVAL;

	hx->addr = b[1] | (b[2] << 8);
	hx->type = b[3];

	if (hx->type == 0x04) {
		/* b[4] and b[5] are the Extended linear address record data field */
		hx->addr |= (b[4] << 24) | (b[5] << 16);
/*		hx->len -= 2;
		data_offs += 2; */
	}
	memcpy(hx->data,&b[data_offs],hx->len);
	hx->chk = b[hx->len + data_offs];

	*pos += hx->len + 5;

	return *pos;
}
