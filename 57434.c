int _yr_emit_inst_arg_struct(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    void* structure,
    size_t structure_size,
    uint8_t** instruction_addr,
    void** argument_addr,
    size_t* code_size)
{
  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &opcode,
      sizeof(uint8_t),
      (void**) instruction_addr));

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      structure,
      structure_size,
      (void**) argument_addr));

  *code_size = sizeof(uint8_t) + structure_size;

  return ERROR_SUCCESS;
}
