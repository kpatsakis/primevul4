static bool sanitize_needed(u8 opcode)
{
	return opcode == BPF_ADD || opcode == BPF_SUB;
}