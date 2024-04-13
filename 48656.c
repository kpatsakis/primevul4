write_str (unsigned char * data, int offset, const char * buffer, int buffer_len)
{	memcpy (data + offset, buffer, buffer_len) ;
} /* write_str */
