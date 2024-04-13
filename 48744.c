bool_t xdr_gss_buf(
     XDR *xdrs,
     gss_buffer_t buf)
{
     /*
      * On decode, xdr_bytes will only allocate buf->value if the
      * length read in is < maxsize (last arg).  This is dumb, because
      * the whole point of allocating memory is so that I don't *have*
      * to know the maximum length.  -1 effectively disables this
      * braindamage.
      */
     bool_t result;
     /* Fix type mismatches between APIs.  */
     unsigned int length = buf->length;
     result = xdr_bytes(xdrs, (char **) &buf->value, &length,
			(xdrs->x_op == XDR_DECODE && buf->value == NULL)
			? (unsigned int) -1 : (unsigned int) buf->length);
     buf->length = length;
     return result;
}
