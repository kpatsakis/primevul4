static void rxrpc_free_token_list(struct rxrpc_key_token *token)
{
	struct rxrpc_key_token *next;

	for (; token; token = next) {
		next = token->next;
		switch (token->security_index) {
		case RXRPC_SECURITY_RXKAD:
			kfree(token->kad);
			break;
		case RXRPC_SECURITY_RXK5:
			if (token->k5)
				rxrpc_rxk5_free(token->k5);
			break;
		default:
			printk(KERN_ERR "Unknown token type %x on rxrpc key\n",
			       token->security_index);
			BUG();
		}

		kfree(token);
	}
}
