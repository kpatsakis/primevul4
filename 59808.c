static void rxrpc_free_krb5_tagged(struct krb5_tagged_data *td)
{
	kfree(td->data);
}
