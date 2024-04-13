static bool convert_bpf_extensions(struct sock_filter *fp,
				   struct sock_filter_int **insnp)
{
	struct sock_filter_int *insn = *insnp;

	switch (fp->k) {
	case SKF_AD_OFF + SKF_AD_PROTOCOL:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, protocol) != 2);

		insn->code = BPF_LDX | BPF_MEM | BPF_H;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, protocol);
		insn++;

		/* A = ntohs(A) [emitting a nop or swap16] */
		insn->code = BPF_ALU | BPF_END | BPF_FROM_BE;
		insn->a_reg = A_REG;
		insn->imm = 16;
		break;

	case SKF_AD_OFF + SKF_AD_PKTTYPE:
		insn->code = BPF_LDX | BPF_MEM | BPF_B;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = pkt_type_offset();
		if (insn->off < 0)
			return false;
		insn++;

		insn->code = BPF_ALU | BPF_AND | BPF_K;
		insn->a_reg = A_REG;
		insn->imm = PKT_TYPE_MAX;
		break;

	case SKF_AD_OFF + SKF_AD_IFINDEX:
	case SKF_AD_OFF + SKF_AD_HATYPE:
		if (FIELD_SIZEOF(struct sk_buff, dev) == 8)
			insn->code = BPF_LDX | BPF_MEM | BPF_DW;
		else
			insn->code = BPF_LDX | BPF_MEM | BPF_W;
		insn->a_reg = TMP_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, dev);
		insn++;

		insn->code = BPF_JMP | BPF_JNE | BPF_K;
		insn->a_reg = TMP_REG;
		insn->imm = 0;
		insn->off = 1;
		insn++;

		insn->code = BPF_JMP | BPF_EXIT;
		insn++;

		BUILD_BUG_ON(FIELD_SIZEOF(struct net_device, ifindex) != 4);
		BUILD_BUG_ON(FIELD_SIZEOF(struct net_device, type) != 2);

		insn->a_reg = A_REG;
		insn->x_reg = TMP_REG;

		if (fp->k == SKF_AD_OFF + SKF_AD_IFINDEX) {
			insn->code = BPF_LDX | BPF_MEM | BPF_W;
			insn->off = offsetof(struct net_device, ifindex);
		} else {
			insn->code = BPF_LDX | BPF_MEM | BPF_H;
			insn->off = offsetof(struct net_device, type);
		}
		break;

	case SKF_AD_OFF + SKF_AD_MARK:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, mark) != 4);

		insn->code = BPF_LDX | BPF_MEM | BPF_W;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, mark);
		break;

	case SKF_AD_OFF + SKF_AD_RXHASH:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, hash) != 4);

		insn->code = BPF_LDX | BPF_MEM | BPF_W;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, hash);
		break;

	case SKF_AD_OFF + SKF_AD_QUEUE:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, queue_mapping) != 2);

		insn->code = BPF_LDX | BPF_MEM | BPF_H;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, queue_mapping);
		break;

	case SKF_AD_OFF + SKF_AD_VLAN_TAG:
	case SKF_AD_OFF + SKF_AD_VLAN_TAG_PRESENT:
		BUILD_BUG_ON(FIELD_SIZEOF(struct sk_buff, vlan_tci) != 2);

		insn->code = BPF_LDX | BPF_MEM | BPF_H;
		insn->a_reg = A_REG;
		insn->x_reg = CTX_REG;
		insn->off = offsetof(struct sk_buff, vlan_tci);
		insn++;

		BUILD_BUG_ON(VLAN_TAG_PRESENT != 0x1000);

		if (fp->k == SKF_AD_OFF + SKF_AD_VLAN_TAG) {
			insn->code = BPF_ALU | BPF_AND | BPF_K;
			insn->a_reg = A_REG;
			insn->imm = ~VLAN_TAG_PRESENT;
		} else {
			insn->code = BPF_ALU | BPF_RSH | BPF_K;
			insn->a_reg = A_REG;
			insn->imm = 12;
			insn++;

			insn->code = BPF_ALU | BPF_AND | BPF_K;
			insn->a_reg = A_REG;
			insn->imm = 1;
		}
		break;

	case SKF_AD_OFF + SKF_AD_PAY_OFFSET:
	case SKF_AD_OFF + SKF_AD_NLATTR:
	case SKF_AD_OFF + SKF_AD_NLATTR_NEST:
	case SKF_AD_OFF + SKF_AD_CPU:
		/* arg1 = ctx */
		insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
		insn->a_reg = ARG1_REG;
		insn->x_reg = CTX_REG;
		insn++;

		/* arg2 = A */
		insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
		insn->a_reg = ARG2_REG;
		insn->x_reg = A_REG;
		insn++;

		/* arg3 = X */
		insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
		insn->a_reg = ARG3_REG;
		insn->x_reg = X_REG;
		insn++;

		/* Emit call(ctx, arg2=A, arg3=X) */
		insn->code = BPF_JMP | BPF_CALL;
		switch (fp->k) {
		case SKF_AD_OFF + SKF_AD_PAY_OFFSET:
			insn->imm = __skb_get_pay_offset - __bpf_call_base;
			break;
		case SKF_AD_OFF + SKF_AD_NLATTR:
			insn->imm = __skb_get_nlattr - __bpf_call_base;
			break;
		case SKF_AD_OFF + SKF_AD_NLATTR_NEST:
			insn->imm = __skb_get_nlattr_nest - __bpf_call_base;
			break;
		case SKF_AD_OFF + SKF_AD_CPU:
			insn->imm = __get_raw_cpu_id - __bpf_call_base;
			break;
		}
		break;

	case SKF_AD_OFF + SKF_AD_ALU_XOR_X:
		insn->code = BPF_ALU | BPF_XOR | BPF_X;
		insn->a_reg = A_REG;
		insn->x_reg = X_REG;
		break;

	default:
		/* This is just a dummy call to avoid letting the compiler
		 * evict __bpf_call_base() as an optimization. Placed here
		 * where no-one bothers.
		 */
		BUG_ON(__bpf_call_base(0, 0, 0, 0, 0) != 0);
		return false;
	}

	*insnp = insn;
	return true;
}
