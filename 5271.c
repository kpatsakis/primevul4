njs_function_constructor(njs_vm_t *vm, njs_value_t *args, njs_uint_t nargs,
    njs_index_t async)
{
    njs_chb_t               chain;
    njs_int_t               ret;
    njs_str_t               str, file;
    njs_uint_t              i;
    njs_lexer_t             lexer;
    njs_parser_t            parser;
    njs_vm_code_t           *code;
    njs_function_t          *function;
    njs_generator_t         generator;
    njs_parser_node_t       *node;
    njs_parser_scope_t      *scope;
    njs_function_lambda_t   *lambda;
    const njs_token_type_t  *type;

    static const njs_token_type_t  safe_ast[] = {
        NJS_TOKEN_END,
        NJS_TOKEN_FUNCTION_EXPRESSION,
        NJS_TOKEN_STATEMENT,
        NJS_TOKEN_RETURN,
        NJS_TOKEN_THIS,
        NJS_TOKEN_ILLEGAL
    };

    static const njs_token_type_t  safe_ast_async[] = {
        NJS_TOKEN_END,
        NJS_TOKEN_ASYNC_FUNCTION_EXPRESSION,
        NJS_TOKEN_STATEMENT,
        NJS_TOKEN_RETURN,
        NJS_TOKEN_THIS,
        NJS_TOKEN_ILLEGAL
    };

    if (!vm->options.unsafe && nargs != 2) {
        goto fail;
    }

    njs_chb_init(&chain, vm->mem_pool);

    if (async) {
        njs_chb_append_literal(&chain, "(async function(");

    } else {
        njs_chb_append_literal(&chain, "(function(");
    }

    for (i = 1; i < nargs - 1; i++) {
        ret = njs_value_to_chain(vm, &chain, njs_argument(args, i));
        if (njs_slow_path(ret < NJS_OK)) {
            return ret;
        }

        if (i != (nargs - 2)) {
            njs_chb_append_literal(&chain, ",");
        }
    }

    njs_chb_append_literal(&chain, "){");

    ret = njs_value_to_chain(vm, &chain, njs_argument(args, nargs - 1));
    if (njs_slow_path(ret < NJS_OK)) {
        return ret;
    }

    njs_chb_append_literal(&chain, "})");

    ret = njs_chb_join(&chain, &str);
    if (njs_slow_path(ret != NJS_OK)) {
        njs_memory_error(vm);
        return NJS_ERROR;
    }

    file = njs_str_value("runtime");

    ret = njs_lexer_init(vm, &lexer, &file, str.start, str.start + str.length,
                         1);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    njs_memzero(&parser, sizeof(njs_parser_t));

    parser.lexer = &lexer;

    ret = njs_parser(vm, &parser);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    if (!vm->options.unsafe) {
        /*
         * Safe mode exception:
         * "(new Function('return this'))" is often used to get
         * the global object in a portable way.
         */

        node = parser.node;
        type = (async) ? &safe_ast_async[0] : &safe_ast[0];

        for (; *type != NJS_TOKEN_ILLEGAL; type++, node = node->right) {
            if (node == NULL) {
                goto fail;
            }

            if (node->left != NULL
                && node->token_type != NJS_TOKEN_FUNCTION_EXPRESSION
                && node->left->token_type != NJS_TOKEN_NAME)
            {
                goto fail;
            }

            if (node->token_type != *type) {
                goto fail;
            }
        }
    }

    scope = parser.scope;

    ret = njs_variables_copy(vm, &scope->variables, vm->variables_hash);
    if (njs_slow_path(ret != NJS_OK)) {
        return ret;
    }

    ret = njs_generator_init(&generator, 0, 1);
    if (njs_slow_path(ret != NJS_OK)) {
        njs_internal_error(vm, "njs_generator_init() failed");
        return NJS_ERROR;
    }

    code = njs_generate_scope(vm, &generator, scope, &njs_entry_anonymous);
    if (njs_slow_path(code == NULL)) {
        if (!njs_is_error(&vm->retval)) {
            njs_internal_error(vm, "njs_generate_scope() failed");
        }

        return NJS_ERROR;
    }

    njs_chb_destroy(&chain);

    lambda = ((njs_vmcode_function_t *) generator.code_start)->lambda;

    function = njs_function_alloc(vm, lambda, (njs_bool_t) async);
    if (njs_slow_path(function == NULL)) {
        return NJS_ERROR;
    }

    function->global = 1;
    function->global_this = 1;
    function->args_count = lambda->nargs - lambda->rest_parameters;

    njs_set_function(&vm->retval, function);

    return NJS_OK;

fail:

    njs_type_error(vm, "function constructor is disabled in \"safe\" mode");
    return NJS_ERROR;
}