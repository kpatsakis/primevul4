static void sctp_v4_del_protocol(void)
{
	inet_del_protocol(&sctp_protocol, IPPROTO_SCTP);
 	unregister_inetaddr_notifier(&sctp_inetaddr_notifier);
 }
