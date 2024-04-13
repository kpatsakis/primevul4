server_send_real (server *serv, char *buf, int len)
{
	fe_add_rawlog (serv, buf, len, TRUE);

	url_check_line (buf, len);

	return tcp_send_real (serv->ssl, serv->sok, serv->encoding, serv->using_irc,
								 buf, len);
}
