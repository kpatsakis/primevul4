void rawsock_exit(void)
{
	nfc_proto_unregister(&rawsock_nfc_proto);
}
