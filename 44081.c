int sk_convert_filter(struct sock_filter *prog, int len,
		      struct sock_filter_int *new_prog, int *new_len)
{
	int new_flen = 0, pass = 0, target, i;
	struct sock_filter_int *new_insn;
	struct sock_filter *fp;
	int *addrs = NULL;
	u8 bpf_src;

	BUILD_BUG_ON(BPF_MEMWORDS * sizeof(u32) > MAX_BPF_STACK);
	BUILD_BUG_ON(FP_REG + 1 != MAX_BPF_REG);

	if (len <= 0 || len >= BPF_MAXINSNS)
		return -EINVAL;

	if (new_prog) {
		addrs = kzalloc(len * sizeof(*addrs), GFP_KERNEL);
		if (!addrs)
			return -ENOMEM;
	}

do_pass:
	new_insn = new_prog;
	fp = prog;

	if (new_insn) {
		new_insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
		new_insn->a_reg = CTX_REG;
		new_insn->x_reg = ARG1_REG;
	}
	new_insn++;

	for (i = 0; i < len; fp++, i++) {
		struct sock_filter_int tmp_insns[6] = { };
		struct sock_filter_int *insn = tmp_insns;

		if (addrs)
			addrs[i] = new_insn - new_prog;

		switch (fp->code) {
		/* All arithmetic insns and skb loads map as-is. */
		case BPF_ALU | BPF_ADD | BPF_X:
		case BPF_ALU | BPF_ADD | BPF_K:
		case BPF_ALU | BPF_SUB | BPF_X:
		case BPF_ALU | BPF_SUB | BPF_K:
		case BPF_ALU | BPF_AND | BPF_X:
		case BPF_ALU | BPF_AND | BPF_K:
		case BPF_ALU | BPF_OR | BPF_X:
		case BPF_ALU | BPF_OR | BPF_K:
		case BPF_ALU | BPF_LSH | BPF_X:
		case BPF_ALU | BPF_LSH | BPF_K:
		case BPF_ALU | BPF_RSH | BPF_X:
		case BPF_ALU | BPF_RSH | BPF_K:
		case BPF_ALU | BPF_XOR | BPF_X:
		case BPF_ALU | BPF_XOR | BPF_K:
		case BPF_ALU | BPF_MUL | BPF_X:
		case BPF_ALU | BPF_MUL | BPF_K:
		case BPF_ALU | BPF_DIV | BPF_X:
		case BPF_ALU | BPF_DIV | BPF_K:
		case BPF_ALU | BPF_MOD | BPF_X:
		case BPF_ALU | BPF_MOD | BPF_K:
		case BPF_ALU | BPF_NEG:
		case BPF_LD | BPF_ABS | BPF_W:
		case BPF_LD | BPF_ABS | BPF_H:
		case BPF_LD | BPF_ABS | BPF_B:
		case BPF_LD | BPF_IND | BPF_W:
		case BPF_LD | BPF_IND | BPF_H:
		case BPF_LD | BPF_IND | BPF_B:
			/* Check for overloaded BPF extension and
			 * directly convert it if found, otherwise
			 * just move on with mapping.
			 */
			if (BPF_CLASS(fp->code) == BPF_LD &&
			    BPF_MODE(fp->code) == BPF_ABS &&
			    convert_bpf_extensions(fp, &insn))
				break;

			insn->code = fp->code;
			insn->a_reg = A_REG;
			insn->x_reg = X_REG;
			insn->imm = fp->k;
			break;

		/* Jump opcodes map as-is, but offsets need adjustment. */
		case BPF_JMP | BPF_JA:
			target = i + fp->k + 1;
			insn->code = fp->code;
#define EMIT_JMP							\
	do {								\
		if (target >= len || target < 0)			\
			goto err;					\
		insn->off = addrs ? addrs[target] - addrs[i] - 1 : 0;	\
		/* Adjust pc relative offset for 2nd or 3rd insn. */	\
		insn->off -= insn - tmp_insns;				\
	} while (0)

			EMIT_JMP;
			break;

		case BPF_JMP | BPF_JEQ | BPF_K:
		case BPF_JMP | BPF_JEQ | BPF_X:
		case BPF_JMP | BPF_JSET | BPF_K:
		case BPF_JMP | BPF_JSET | BPF_X:
		case BPF_JMP | BPF_JGT | BPF_K:
		case BPF_JMP | BPF_JGT | BPF_X:
		case BPF_JMP | BPF_JGE | BPF_K:
		case BPF_JMP | BPF_JGE | BPF_X:
			if (BPF_SRC(fp->code) == BPF_K && (int) fp->k < 0) {
				/* BPF immediates are signed, zero extend
				 * immediate into tmp register and use it
				 * in compare insn.
				 */
				insn->code = BPF_ALU | BPF_MOV | BPF_K;
				insn->a_reg = TMP_REG;
				insn->imm = fp->k;
				insn++;

				insn->a_reg = A_REG;
				insn->x_reg = TMP_REG;
				bpf_src = BPF_X;
			} else {
				insn->a_reg = A_REG;
				insn->x_reg = X_REG;
				insn->imm = fp->k;
				bpf_src = BPF_SRC(fp->code);
			}

			/* Common case where 'jump_false' is next insn. */
			if (fp->jf == 0) {
				insn->code = BPF_JMP | BPF_OP(fp->code) | bpf_src;
				target = i + fp->jt + 1;
				EMIT_JMP;
				break;
			}

			/* Convert JEQ into JNE when 'jump_true' is next insn. */
			if (fp->jt == 0 && BPF_OP(fp->code) == BPF_JEQ) {
				insn->code = BPF_JMP | BPF_JNE | bpf_src;
				target = i + fp->jf + 1;
				EMIT_JMP;
				break;
			}

			/* Other jumps are mapped into two insns: Jxx and JA. */
			target = i + fp->jt + 1;
			insn->code = BPF_JMP | BPF_OP(fp->code) | bpf_src;
			EMIT_JMP;
			insn++;

			insn->code = BPF_JMP | BPF_JA;
			target = i + fp->jf + 1;
			EMIT_JMP;
			break;

		/* ldxb 4 * ([14] & 0xf) is remaped into 6 insns. */
		case BPF_LDX | BPF_MSH | BPF_B:
			insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
			insn->a_reg = TMP_REG;
			insn->x_reg = A_REG;
			insn++;

			insn->code = BPF_LD | BPF_ABS | BPF_B;
			insn->a_reg = A_REG;
			insn->imm = fp->k;
			insn++;

			insn->code = BPF_ALU | BPF_AND | BPF_K;
			insn->a_reg = A_REG;
			insn->imm = 0xf;
			insn++;

			insn->code = BPF_ALU | BPF_LSH | BPF_K;
			insn->a_reg = A_REG;
			insn->imm = 2;
			insn++;

			insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
			insn->a_reg = X_REG;
			insn->x_reg = A_REG;
			insn++;

			insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
			insn->a_reg = A_REG;
			insn->x_reg = TMP_REG;
			break;

		/* RET_K, RET_A are remaped into 2 insns. */
		case BPF_RET | BPF_A:
		case BPF_RET | BPF_K:
			insn->code = BPF_ALU | BPF_MOV |
				     (BPF_RVAL(fp->code) == BPF_K ?
				      BPF_K : BPF_X);
			insn->a_reg = 0;
			insn->x_reg = A_REG;
			insn->imm = fp->k;
			insn++;

			insn->code = BPF_JMP | BPF_EXIT;
			break;

		/* Store to stack. */
		case BPF_ST:
		case BPF_STX:
			insn->code = BPF_STX | BPF_MEM | BPF_W;
			insn->a_reg = FP_REG;
			insn->x_reg = fp->code == BPF_ST ? A_REG : X_REG;
			insn->off = -(BPF_MEMWORDS - fp->k) * 4;
			break;

		/* Load from stack. */
		case BPF_LD | BPF_MEM:
		case BPF_LDX | BPF_MEM:
			insn->code = BPF_LDX | BPF_MEM | BPF_W;
			insn->a_reg = BPF_CLASS(fp->code) == BPF_LD ?
				      A_REG : X_REG;
			insn->x_reg = FP_REG;
			insn->off = -(BPF_MEMWORDS - fp->k) * 4;
			break;

		/* A = K or X = K */
		case BPF_LD | BPF_IMM:
		case BPF_LDX | BPF_IMM:
			insn->code = BPF_ALU | BPF_MOV | BPF_K;
			insn->a_reg = BPF_CLASS(fp->code) == BPF_LD ?
				      A_REG : X_REG;
			insn->imm = fp->k;
			break;

		/* X = A */
		case BPF_MISC | BPF_TAX:
			insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
			insn->a_reg = X_REG;
			insn->x_reg = A_REG;
			break;

		/* A = X */
		case BPF_MISC | BPF_TXA:
			insn->code = BPF_ALU64 | BPF_MOV | BPF_X;
			insn->a_reg = A_REG;
			insn->x_reg = X_REG;
			break;

		/* A = skb->len or X = skb->len */
		case BPF_LD | BPF_W | BPF_LEN:
		case BPF_LDX | BPF_W | BPF_LEN:
			insn->code = BPF_LDX | BPF_MEM | BPF_W;
			insn->a_reg = BPF_CLASS(fp->code) == BPF_LD ?
				      A_REG : X_REG;
			insn->x_reg = CTX_REG;
			insn->off = offsetof(struct sk_buff, len);
			break;

		/* access seccomp_data fields */
		case BPF_LDX | BPF_ABS | BPF_W:
			insn->code = BPF_LDX | BPF_MEM | BPF_W;
			insn->a_reg = A_REG;
			insn->x_reg = CTX_REG;
			insn->off = fp->k;
			break;

		default:
			goto err;
		}

		insn++;
		if (new_prog)
			memcpy(new_insn, tmp_insns,
			       sizeof(*insn) * (insn - tmp_insns));

		new_insn += insn - tmp_insns;
	}

	if (!new_prog) {
		/* Only calculating new length. */
		*new_len = new_insn - new_prog;
		return 0;
	}

	pass++;
	if (new_flen != new_insn - new_prog) {
		new_flen = new_insn - new_prog;
		if (pass > 2)
			goto err;

		goto do_pass;
	}

	kfree(addrs);
	BUG_ON(*new_len != new_flen);
	return 0;
err:
	kfree(addrs);
	return -EINVAL;
}
