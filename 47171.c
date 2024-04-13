static int oz_hcd_buffer_data(struct oz_endpoint *ep, const u8 *data,
			      int data_len)
{
	int space;
	int copy_len;

	if (!ep->buffer)
		return -1;
	space = ep->out_ix-ep->in_ix-1;
	if (space < 0)
		space += ep->buffer_size;
	if (space < (data_len+1)) {
		oz_dbg(ON, "Buffer full\n");
		return -1;
	}
	ep->buffer[ep->in_ix] = (u8)data_len;
	if (++ep->in_ix == ep->buffer_size)
		ep->in_ix = 0;
	copy_len = ep->buffer_size - ep->in_ix;
	if (copy_len > data_len)
		copy_len = data_len;
	memcpy(&ep->buffer[ep->in_ix], data, copy_len);

	if (copy_len < data_len) {
		memcpy(ep->buffer, data+copy_len, data_len-copy_len);
		ep->in_ix = data_len-copy_len;
	} else {
		ep->in_ix += copy_len;
	}
	if (ep->in_ix == ep->buffer_size)
		ep->in_ix = 0;
	ep->buffered_units++;
	return 0;
}
