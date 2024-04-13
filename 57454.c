int yr_re_compile(
    const char* re_string,
    int flags,
    YR_ARENA* code_arena,
    RE** re,
    RE_ERROR* error)
{
  RE_AST* re_ast;
  RE _re;

  FAIL_ON_ERROR(yr_arena_reserve_memory(
      code_arena, sizeof(int64_t) + RE_MAX_CODE_SIZE));

  FAIL_ON_ERROR(yr_re_parse(re_string, &re_ast, error));

  _re.flags = flags;

  FAIL_ON_ERROR_WITH_CLEANUP(
      yr_arena_write_data(
          code_arena,
          &_re,
          sizeof(_re),
          (void**) re),
      yr_re_ast_destroy(re_ast));

  FAIL_ON_ERROR_WITH_CLEANUP(
      yr_re_ast_emit_code(re_ast, code_arena, FALSE),
      yr_re_ast_destroy(re_ast));

  yr_re_ast_destroy(re_ast);

  return ERROR_SUCCESS;
}
