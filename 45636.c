int __init rawsock_init(void)
{
	int rc;

	rc = nfc_proto_register(&rawsock_nfc_proto);

	return rc;
}
