static bool is_spillable_regtype(enum bpf_reg_type type)
{
	switch (type) {
	case PTR_TO_MAP_VALUE:
	case PTR_TO_MAP_VALUE_OR_NULL:
	case PTR_TO_MAP_VALUE_ADJ:
	case PTR_TO_STACK:
	case PTR_TO_CTX:
	case PTR_TO_PACKET:
	case PTR_TO_PACKET_END:
	case FRAME_PTR:
	case CONST_PTR_TO_MAP:
		return true;
	default:
		return false;
	}
}
