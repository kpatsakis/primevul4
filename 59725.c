void unregister_asymmetric_key_parser(struct asymmetric_key_parser *parser)
{
	down_write(&asymmetric_key_parsers_sem);
	list_del(&parser->link);
	up_write(&asymmetric_key_parsers_sem);

	pr_notice("Asymmetric key parser '%s' unregistered\n", parser->name);
}
