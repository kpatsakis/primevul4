void selinux_audit_rule_free(void *vrule)
{
	struct selinux_audit_rule *rule = vrule;

	if (rule) {
		context_destroy(&rule->au_ctxt);
		kfree(rule);
	}
}
