int _yr_re_emit(
    RE_EMIT_CONTEXT* emit_context,
    RE_NODE* re_node,
    int flags,
    uint8_t** code_addr,
    size_t* code_size)
{
  size_t branch_size;
  size_t split_size;
  size_t inst_size;
  size_t jmp_size;

  int emit_split;
  int emit_repeat;
  int emit_prolog;
  int emit_epilog;

  RE_REPEAT_ARGS repeat_args;
  RE_REPEAT_ARGS* repeat_start_args_addr;
  RE_REPEAT_ANY_ARGS repeat_any_args;

  RE_NODE* left;
  RE_NODE* right;

  int16_t* split_offset_addr = NULL;
  int16_t* jmp_offset_addr = NULL;
  uint8_t* instruction_addr = NULL;

  *code_size = 0;

  switch(re_node->type)
  {
  case RE_NODE_LITERAL:

    FAIL_ON_ERROR(_yr_emit_inst_arg_uint8(
        emit_context,
        RE_OPCODE_LITERAL,
        re_node->value,
        &instruction_addr,
        NULL,
        code_size));
    break;

  case RE_NODE_MASKED_LITERAL:

    FAIL_ON_ERROR(_yr_emit_inst_arg_uint16(
        emit_context,
        RE_OPCODE_MASKED_LITERAL,
        re_node->mask << 8 | re_node->value,
        &instruction_addr,
        NULL,
        code_size));
    break;

  case RE_NODE_WORD_CHAR:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_WORD_CHAR,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_NON_WORD_CHAR:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_NON_WORD_CHAR,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_WORD_BOUNDARY:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_WORD_BOUNDARY,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_NON_WORD_BOUNDARY:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_NON_WORD_BOUNDARY,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_SPACE:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_SPACE,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_NON_SPACE:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_NON_SPACE,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_DIGIT:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_DIGIT,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_NON_DIGIT:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_NON_DIGIT,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_ANY:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_ANY,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_CLASS:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_CLASS,
        &instruction_addr,
        code_size));

    FAIL_ON_ERROR(yr_arena_write_data(
        emit_context->arena,
        re_node->class_vector,
        32,
        NULL));

    *code_size += 32;
    break;

  case RE_NODE_ANCHOR_START:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_MATCH_AT_START,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_ANCHOR_END:

    FAIL_ON_ERROR(_yr_emit_inst(
        emit_context,
        RE_OPCODE_MATCH_AT_END,
        &instruction_addr,
        code_size));
    break;

  case RE_NODE_CONCAT:

    if (flags & EMIT_BACKWARDS)
    {
      left = re_node->right;
      right = re_node->left;
    }
    else
    {
      left = re_node->left;
      right = re_node->right;
    }

    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        left,
        flags,
        &instruction_addr,
        &branch_size));

    *code_size += branch_size;

    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        right,
        flags,
        NULL,
        &branch_size));

    *code_size += branch_size;

    break;

  case RE_NODE_PLUS:


    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        re_node->left,
        flags,
        &instruction_addr,
        &branch_size));

    *code_size += branch_size;

    FAIL_ON_ERROR(_yr_emit_split(
        emit_context,
        re_node->greedy ? RE_OPCODE_SPLIT_B : RE_OPCODE_SPLIT_A,
        -((int16_t) branch_size),
        NULL,
        &split_offset_addr,
        &split_size));

    *code_size += split_size;
    break;

  case RE_NODE_STAR:


    FAIL_ON_ERROR(_yr_emit_split(
        emit_context,
        re_node->greedy ? RE_OPCODE_SPLIT_A : RE_OPCODE_SPLIT_B,
        0,
        &instruction_addr,
        &split_offset_addr,
        &split_size));

    *code_size += split_size;

    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        re_node->left,
        flags,
        NULL,
        &branch_size));

    *code_size += branch_size;


    FAIL_ON_ERROR(_yr_emit_inst_arg_int16(
        emit_context,
        RE_OPCODE_JUMP,
        -((uint16_t)(branch_size + split_size)),
        NULL,
        &jmp_offset_addr,
        &jmp_size));

    *code_size += jmp_size;

    assert(split_size + branch_size + jmp_size < INT16_MAX);

    *split_offset_addr = (int16_t) (split_size + branch_size + jmp_size);
    break;

  case RE_NODE_ALT:



    FAIL_ON_ERROR(_yr_emit_split(
        emit_context,
        RE_OPCODE_SPLIT_A,
        0,
        &instruction_addr,
        &split_offset_addr,
        &split_size));

    *code_size += split_size;

    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        re_node->left,
        flags,
        NULL,
        &branch_size));

    *code_size += branch_size;


    FAIL_ON_ERROR(_yr_emit_inst_arg_int16(
        emit_context,
        RE_OPCODE_JUMP,
        0,
        NULL,
        &jmp_offset_addr,
        &jmp_size));

    *code_size += jmp_size;

    assert(split_size + branch_size + jmp_size < INT16_MAX);

    *split_offset_addr = (int16_t) (split_size + branch_size + jmp_size);

    FAIL_ON_ERROR(_yr_re_emit(
        emit_context,
        re_node->right,
        flags,
        NULL,
        &branch_size));

    *code_size += branch_size;

    assert(branch_size + jmp_size < INT16_MAX);

    *jmp_offset_addr = (int16_t) (branch_size + jmp_size);
    break;

  case RE_NODE_RANGE_ANY:

    repeat_any_args.min = re_node->start;
    repeat_any_args.max = re_node->end;

    FAIL_ON_ERROR(_yr_emit_inst_arg_struct(
        emit_context,
        re_node->greedy ?
            RE_OPCODE_REPEAT_ANY_GREEDY :
            RE_OPCODE_REPEAT_ANY_UNGREEDY,
        &repeat_any_args,
        sizeof(repeat_any_args),
        &instruction_addr,
        NULL,
        &inst_size));

    *code_size += inst_size;
    break;

  case RE_NODE_RANGE:


    emit_prolog = re_node->start > 0;
    emit_repeat = re_node->end > re_node->start + 1 || re_node->end > 2;
    emit_split = re_node->end > re_node->start;
    emit_epilog = re_node->end > re_node->start || re_node->end > 1;

    if (emit_prolog)
    {
      FAIL_ON_ERROR(_yr_re_emit(
          emit_context,
          re_node->left,
          flags,
          &instruction_addr,
          &branch_size));

       *code_size += branch_size;
    }

    if (emit_repeat)
    {
      repeat_args.min = re_node->start;
      repeat_args.max = re_node->end;

      if (emit_prolog)
      {
        repeat_args.max--;
        repeat_args.min--;
      }

      if (emit_split)
        repeat_args.max--;
      else
        repeat_args.min--;

      repeat_args.offset = 0;

      FAIL_ON_ERROR(_yr_emit_inst_arg_struct(
          emit_context,
          re_node->greedy ?
              RE_OPCODE_REPEAT_START_GREEDY :
              RE_OPCODE_REPEAT_START_UNGREEDY,
          &repeat_args,
          sizeof(repeat_args),
          emit_prolog ? NULL : &instruction_addr,
          (void**) &repeat_start_args_addr,
          &inst_size));

      *code_size += inst_size;

      FAIL_ON_ERROR(_yr_re_emit(
          emit_context,
          re_node->left,
          flags | EMIT_DONT_SET_FORWARDS_CODE | EMIT_DONT_SET_BACKWARDS_CODE,
          NULL,
          &branch_size));

      *code_size += branch_size;

      repeat_start_args_addr->offset = (int32_t)(2 * inst_size + branch_size);
      repeat_args.offset = -((int32_t) branch_size);

      FAIL_ON_ERROR(_yr_emit_inst_arg_struct(
          emit_context,
          re_node->greedy ?
              RE_OPCODE_REPEAT_END_GREEDY :
              RE_OPCODE_REPEAT_END_UNGREEDY,
          &repeat_args,
          sizeof(repeat_args),
          NULL,
          NULL,
          &inst_size));

      *code_size += inst_size;
    }

    if (emit_split)
    {
      FAIL_ON_ERROR(_yr_emit_split(
          emit_context,
          re_node->greedy ?
              RE_OPCODE_SPLIT_A :
              RE_OPCODE_SPLIT_B,
          0,
          NULL,
          &split_offset_addr,
          &split_size));

      *code_size += split_size;
    }

    if (emit_epilog)
    {
      FAIL_ON_ERROR(_yr_re_emit(
          emit_context,
          re_node->left,
          emit_prolog ? flags | EMIT_DONT_SET_FORWARDS_CODE : flags,
          emit_prolog || emit_repeat ? NULL : &instruction_addr,
          &branch_size));

      *code_size += branch_size;
    }

    if (emit_split)
    {
      assert(split_size + branch_size  < INT16_MAX);
      *split_offset_addr = (int16_t) (split_size + branch_size);
    }

    break;
  }

  if (flags & EMIT_BACKWARDS)
  {
    if (!(flags & EMIT_DONT_SET_BACKWARDS_CODE))
      re_node->backward_code = instruction_addr + *code_size;
  }
  else
  {
    if (!(flags & EMIT_DONT_SET_FORWARDS_CODE))
      re_node->forward_code = instruction_addr;
  }

  if (code_addr != NULL)
    *code_addr = instruction_addr;

  return ERROR_SUCCESS;
}
