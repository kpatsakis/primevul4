static struct iscsi_param *iscsi_set_default_param(struct iscsi_param_list *param_list,
		char *name, char *value, u8 phase, u8 scope, u8 sender,
		u16 type_range, u8 use)
{
	struct iscsi_param *param = NULL;

	param = kzalloc(sizeof(struct iscsi_param), GFP_KERNEL);
	if (!param) {
		pr_err("Unable to allocate memory for parameter.\n");
		goto out;
	}
	INIT_LIST_HEAD(&param->p_list);

	param->name = kstrdup(name, GFP_KERNEL);
	if (!param->name) {
		pr_err("Unable to allocate memory for parameter name.\n");
		goto out;
	}

	param->value = kstrdup(value, GFP_KERNEL);
	if (!param->value) {
		pr_err("Unable to allocate memory for parameter value.\n");
		goto out;
	}

	param->phase		= phase;
	param->scope		= scope;
	param->sender		= sender;
	param->use		= use;
	param->type_range	= type_range;

	switch (param->type_range) {
	case TYPERANGE_BOOL_AND:
		param->type = TYPE_BOOL_AND;
		break;
	case TYPERANGE_BOOL_OR:
		param->type = TYPE_BOOL_OR;
		break;
	case TYPERANGE_0_TO_2:
	case TYPERANGE_0_TO_3600:
	case TYPERANGE_0_TO_32767:
	case TYPERANGE_0_TO_65535:
	case TYPERANGE_1_TO_65535:
	case TYPERANGE_2_TO_3600:
	case TYPERANGE_512_TO_16777215:
		param->type = TYPE_NUMBER;
		break;
	case TYPERANGE_AUTH:
	case TYPERANGE_DIGEST:
		param->type = TYPE_VALUE_LIST | TYPE_STRING;
		break;
	case TYPERANGE_MARKINT:
		param->type = TYPE_NUMBER_RANGE;
		param->type_range |= TYPERANGE_1_TO_65535;
		break;
	case TYPERANGE_ISCSINAME:
	case TYPERANGE_SESSIONTYPE:
	case TYPERANGE_TARGETADDRESS:
	case TYPERANGE_UTF8:
		param->type = TYPE_STRING;
		break;
	default:
		pr_err("Unknown type_range 0x%02x\n",
				param->type_range);
		goto out;
	}
	list_add_tail(&param->p_list, &param_list->param_list);

	return param;
out:
	if (param) {
		kfree(param->value);
		kfree(param->name);
		kfree(param);
	}

	return NULL;
}
