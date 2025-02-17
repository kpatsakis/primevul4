unsigned int __sk_run_filter(void *ctx, const struct sock_filter_int *insn)
{
	u64 stack[MAX_BPF_STACK / sizeof(u64)];
	u64 regs[MAX_BPF_REG], tmp;
	void *ptr;
	int off;

#define K  insn->imm
#define A  regs[insn->a_reg]
#define X  regs[insn->x_reg]
#define R0 regs[0]

#define CONT	 ({insn++; goto select_insn; })
#define CONT_JMP ({insn++; goto select_insn; })

	static const void *jumptable[256] = {
		[0 ... 255] = &&default_label,
		/* Now overwrite non-defaults ... */
#define DL(A, B, C)	[A|B|C] = &&A##_##B##_##C
		DL(BPF_ALU, BPF_ADD, BPF_X),
		DL(BPF_ALU, BPF_ADD, BPF_K),
		DL(BPF_ALU, BPF_SUB, BPF_X),
		DL(BPF_ALU, BPF_SUB, BPF_K),
		DL(BPF_ALU, BPF_AND, BPF_X),
		DL(BPF_ALU, BPF_AND, BPF_K),
		DL(BPF_ALU, BPF_OR, BPF_X),
		DL(BPF_ALU, BPF_OR, BPF_K),
		DL(BPF_ALU, BPF_LSH, BPF_X),
		DL(BPF_ALU, BPF_LSH, BPF_K),
		DL(BPF_ALU, BPF_RSH, BPF_X),
		DL(BPF_ALU, BPF_RSH, BPF_K),
		DL(BPF_ALU, BPF_XOR, BPF_X),
		DL(BPF_ALU, BPF_XOR, BPF_K),
		DL(BPF_ALU, BPF_MUL, BPF_X),
		DL(BPF_ALU, BPF_MUL, BPF_K),
		DL(BPF_ALU, BPF_MOV, BPF_X),
		DL(BPF_ALU, BPF_MOV, BPF_K),
		DL(BPF_ALU, BPF_DIV, BPF_X),
		DL(BPF_ALU, BPF_DIV, BPF_K),
		DL(BPF_ALU, BPF_MOD, BPF_X),
		DL(BPF_ALU, BPF_MOD, BPF_K),
		DL(BPF_ALU, BPF_NEG, 0),
		DL(BPF_ALU, BPF_END, BPF_TO_BE),
		DL(BPF_ALU, BPF_END, BPF_TO_LE),
		DL(BPF_ALU64, BPF_ADD, BPF_X),
		DL(BPF_ALU64, BPF_ADD, BPF_K),
		DL(BPF_ALU64, BPF_SUB, BPF_X),
		DL(BPF_ALU64, BPF_SUB, BPF_K),
		DL(BPF_ALU64, BPF_AND, BPF_X),
		DL(BPF_ALU64, BPF_AND, BPF_K),
		DL(BPF_ALU64, BPF_OR, BPF_X),
		DL(BPF_ALU64, BPF_OR, BPF_K),
		DL(BPF_ALU64, BPF_LSH, BPF_X),
		DL(BPF_ALU64, BPF_LSH, BPF_K),
		DL(BPF_ALU64, BPF_RSH, BPF_X),
		DL(BPF_ALU64, BPF_RSH, BPF_K),
		DL(BPF_ALU64, BPF_XOR, BPF_X),
		DL(BPF_ALU64, BPF_XOR, BPF_K),
		DL(BPF_ALU64, BPF_MUL, BPF_X),
		DL(BPF_ALU64, BPF_MUL, BPF_K),
		DL(BPF_ALU64, BPF_MOV, BPF_X),
		DL(BPF_ALU64, BPF_MOV, BPF_K),
		DL(BPF_ALU64, BPF_ARSH, BPF_X),
		DL(BPF_ALU64, BPF_ARSH, BPF_K),
		DL(BPF_ALU64, BPF_DIV, BPF_X),
		DL(BPF_ALU64, BPF_DIV, BPF_K),
		DL(BPF_ALU64, BPF_MOD, BPF_X),
		DL(BPF_ALU64, BPF_MOD, BPF_K),
		DL(BPF_ALU64, BPF_NEG, 0),
		DL(BPF_JMP, BPF_CALL, 0),
		DL(BPF_JMP, BPF_JA, 0),
		DL(BPF_JMP, BPF_JEQ, BPF_X),
		DL(BPF_JMP, BPF_JEQ, BPF_K),
		DL(BPF_JMP, BPF_JNE, BPF_X),
		DL(BPF_JMP, BPF_JNE, BPF_K),
		DL(BPF_JMP, BPF_JGT, BPF_X),
		DL(BPF_JMP, BPF_JGT, BPF_K),
		DL(BPF_JMP, BPF_JGE, BPF_X),
		DL(BPF_JMP, BPF_JGE, BPF_K),
		DL(BPF_JMP, BPF_JSGT, BPF_X),
		DL(BPF_JMP, BPF_JSGT, BPF_K),
		DL(BPF_JMP, BPF_JSGE, BPF_X),
		DL(BPF_JMP, BPF_JSGE, BPF_K),
		DL(BPF_JMP, BPF_JSET, BPF_X),
		DL(BPF_JMP, BPF_JSET, BPF_K),
		DL(BPF_JMP, BPF_EXIT, 0),
		DL(BPF_STX, BPF_MEM, BPF_B),
		DL(BPF_STX, BPF_MEM, BPF_H),
		DL(BPF_STX, BPF_MEM, BPF_W),
		DL(BPF_STX, BPF_MEM, BPF_DW),
		DL(BPF_STX, BPF_XADD, BPF_W),
		DL(BPF_STX, BPF_XADD, BPF_DW),
		DL(BPF_ST, BPF_MEM, BPF_B),
		DL(BPF_ST, BPF_MEM, BPF_H),
		DL(BPF_ST, BPF_MEM, BPF_W),
		DL(BPF_ST, BPF_MEM, BPF_DW),
		DL(BPF_LDX, BPF_MEM, BPF_B),
		DL(BPF_LDX, BPF_MEM, BPF_H),
		DL(BPF_LDX, BPF_MEM, BPF_W),
		DL(BPF_LDX, BPF_MEM, BPF_DW),
		DL(BPF_LD, BPF_ABS, BPF_W),
		DL(BPF_LD, BPF_ABS, BPF_H),
		DL(BPF_LD, BPF_ABS, BPF_B),
		DL(BPF_LD, BPF_IND, BPF_W),
		DL(BPF_LD, BPF_IND, BPF_H),
		DL(BPF_LD, BPF_IND, BPF_B),
#undef DL
	};

	regs[FP_REG]  = (u64) (unsigned long) &stack[ARRAY_SIZE(stack)];
	regs[ARG1_REG] = (u64) (unsigned long) ctx;

select_insn:
	goto *jumptable[insn->code];

	/* ALU */
#define ALU(OPCODE, OP)			\
	BPF_ALU64_##OPCODE##_BPF_X:	\
		A = A OP X;		\
		CONT;			\
	BPF_ALU_##OPCODE##_BPF_X:	\
		A = (u32) A OP (u32) X;	\
		CONT;			\
	BPF_ALU64_##OPCODE##_BPF_K:	\
		A = A OP K;		\
		CONT;			\
	BPF_ALU_##OPCODE##_BPF_K:	\
		A = (u32) A OP (u32) K;	\
		CONT;

	ALU(BPF_ADD,  +)
	ALU(BPF_SUB,  -)
	ALU(BPF_AND,  &)
	ALU(BPF_OR,   |)
	ALU(BPF_LSH, <<)
	ALU(BPF_RSH, >>)
	ALU(BPF_XOR,  ^)
	ALU(BPF_MUL,  *)
#undef ALU
	BPF_ALU_BPF_NEG_0:
		A = (u32) -A;
		CONT;
	BPF_ALU64_BPF_NEG_0:
		A = -A;
		CONT;
	BPF_ALU_BPF_MOV_BPF_X:
		A = (u32) X;
		CONT;
	BPF_ALU_BPF_MOV_BPF_K:
		A = (u32) K;
		CONT;
	BPF_ALU64_BPF_MOV_BPF_X:
		A = X;
		CONT;
	BPF_ALU64_BPF_MOV_BPF_K:
		A = K;
		CONT;
	BPF_ALU64_BPF_ARSH_BPF_X:
		(*(s64 *) &A) >>= X;
		CONT;
	BPF_ALU64_BPF_ARSH_BPF_K:
		(*(s64 *) &A) >>= K;
		CONT;
	BPF_ALU64_BPF_MOD_BPF_X:
		if (unlikely(X == 0))
			return 0;
		tmp = A;
		A = do_div(tmp, X);
		CONT;
	BPF_ALU_BPF_MOD_BPF_X:
		if (unlikely(X == 0))
			return 0;
		tmp = (u32) A;
		A = do_div(tmp, (u32) X);
		CONT;
	BPF_ALU64_BPF_MOD_BPF_K:
		tmp = A;
		A = do_div(tmp, K);
		CONT;
	BPF_ALU_BPF_MOD_BPF_K:
		tmp = (u32) A;
		A = do_div(tmp, (u32) K);
		CONT;
	BPF_ALU64_BPF_DIV_BPF_X:
		if (unlikely(X == 0))
			return 0;
		do_div(A, X);
		CONT;
	BPF_ALU_BPF_DIV_BPF_X:
		if (unlikely(X == 0))
			return 0;
		tmp = (u32) A;
		do_div(tmp, (u32) X);
		A = (u32) tmp;
		CONT;
	BPF_ALU64_BPF_DIV_BPF_K:
		do_div(A, K);
		CONT;
	BPF_ALU_BPF_DIV_BPF_K:
		tmp = (u32) A;
		do_div(tmp, (u32) K);
		A = (u32) tmp;
		CONT;
	BPF_ALU_BPF_END_BPF_TO_BE:
		switch (K) {
		case 16:
			A = (__force u16) cpu_to_be16(A);
			break;
		case 32:
			A = (__force u32) cpu_to_be32(A);
			break;
		case 64:
			A = (__force u64) cpu_to_be64(A);
			break;
		}
		CONT;
	BPF_ALU_BPF_END_BPF_TO_LE:
		switch (K) {
		case 16:
			A = (__force u16) cpu_to_le16(A);
			break;
		case 32:
			A = (__force u32) cpu_to_le32(A);
			break;
		case 64:
			A = (__force u64) cpu_to_le64(A);
			break;
		}
		CONT;

	/* CALL */
	BPF_JMP_BPF_CALL_0:
		/* Function call scratches R1-R5 registers, preserves R6-R9,
		 * and stores return value into R0.
		 */
		R0 = (__bpf_call_base + insn->imm)(regs[1], regs[2], regs[3],
						   regs[4], regs[5]);
		CONT;

	/* JMP */
	BPF_JMP_BPF_JA_0:
		insn += insn->off;
		CONT;
	BPF_JMP_BPF_JEQ_BPF_X:
		if (A == X) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JEQ_BPF_K:
		if (A == K) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JNE_BPF_X:
		if (A != X) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JNE_BPF_K:
		if (A != K) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JGT_BPF_X:
		if (A > X) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JGT_BPF_K:
		if (A > K) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JGE_BPF_X:
		if (A >= X) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JGE_BPF_K:
		if (A >= K) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSGT_BPF_X:
		if (((s64)A) > ((s64)X)) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSGT_BPF_K:
		if (((s64)A) > ((s64)K)) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSGE_BPF_X:
		if (((s64)A) >= ((s64)X)) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSGE_BPF_K:
		if (((s64)A) >= ((s64)K)) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSET_BPF_X:
		if (A & X) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_JSET_BPF_K:
		if (A & K) {
			insn += insn->off;
			CONT_JMP;
		}
		CONT;
	BPF_JMP_BPF_EXIT_0:
		return R0;

	/* STX and ST and LDX*/
#define LDST(SIZEOP, SIZE)					\
	BPF_STX_BPF_MEM_##SIZEOP:				\
		*(SIZE *)(unsigned long) (A + insn->off) = X;	\
		CONT;						\
	BPF_ST_BPF_MEM_##SIZEOP:				\
		*(SIZE *)(unsigned long) (A + insn->off) = K;	\
		CONT;						\
	BPF_LDX_BPF_MEM_##SIZEOP:				\
		A = *(SIZE *)(unsigned long) (X + insn->off);	\
		CONT;

	LDST(BPF_B,   u8)
	LDST(BPF_H,  u16)
	LDST(BPF_W,  u32)
	LDST(BPF_DW, u64)
#undef LDST
	BPF_STX_BPF_XADD_BPF_W: /* lock xadd *(u32 *)(A + insn->off) += X */
		atomic_add((u32) X, (atomic_t *)(unsigned long)
			   (A + insn->off));
		CONT;
	BPF_STX_BPF_XADD_BPF_DW: /* lock xadd *(u64 *)(A + insn->off) += X */
		atomic64_add((u64) X, (atomic64_t *)(unsigned long)
			     (A + insn->off));
		CONT;
	BPF_LD_BPF_ABS_BPF_W: /* R0 = ntohl(*(u32 *) (skb->data + K)) */
		off = K;
load_word:
		/* BPF_LD + BPD_ABS and BPF_LD + BPF_IND insns are only
		 * appearing in the programs where ctx == skb. All programs
		 * keep 'ctx' in regs[CTX_REG] == R6, sk_convert_filter()
		 * saves it in R6, internal BPF verifier will check that
		 * R6 == ctx.
		 *
		 * BPF_ABS and BPF_IND are wrappers of function calls, so
		 * they scratch R1-R5 registers, preserve R6-R9, and store
		 * return value into R0.
		 *
		 * Implicit input:
		 *   ctx
		 *
		 * Explicit input:
		 *   X == any register
		 *   K == 32-bit immediate
		 *
		 * Output:
		 *   R0 - 8/16/32-bit skb data converted to cpu endianness
		 */
		ptr = load_pointer((struct sk_buff *) ctx, off, 4, &tmp);
		if (likely(ptr != NULL)) {
			R0 = get_unaligned_be32(ptr);
			CONT;
		}
		return 0;
	BPF_LD_BPF_ABS_BPF_H: /* R0 = ntohs(*(u16 *) (skb->data + K)) */
		off = K;
load_half:
		ptr = load_pointer((struct sk_buff *) ctx, off, 2, &tmp);
		if (likely(ptr != NULL)) {
			R0 = get_unaligned_be16(ptr);
			CONT;
		}
		return 0;
	BPF_LD_BPF_ABS_BPF_B: /* R0 = *(u8 *) (ctx + K) */
		off = K;
load_byte:
		ptr = load_pointer((struct sk_buff *) ctx, off, 1, &tmp);
		if (likely(ptr != NULL)) {
			R0 = *(u8 *)ptr;
			CONT;
		}
		return 0;
	BPF_LD_BPF_IND_BPF_W: /* R0 = ntohl(*(u32 *) (skb->data + X + K)) */
		off = K + X;
		goto load_word;
	BPF_LD_BPF_IND_BPF_H: /* R0 = ntohs(*(u16 *) (skb->data + X + K)) */
		off = K + X;
		goto load_half;
	BPF_LD_BPF_IND_BPF_B: /* R0 = *(u8 *) (skb->data + X + K) */
		off = K + X;
		goto load_byte;

	default_label:
		/* If we ever reach this, we have a bug somewhere. */
		WARN_RATELIMIT(1, "unknown opcode %02x\n", insn->code);
		return 0;
#undef CONT_JMP
#undef CONT

#undef R0
#undef X
#undef A
#undef K
}
