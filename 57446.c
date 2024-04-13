int _yr_re_fiber_sync(
    RE_FIBER_LIST* fiber_list,
    RE_FIBER_POOL* fiber_pool,
    RE_FIBER* fiber_to_sync)
{

  RE_SPLIT_ID_TYPE splits_executed[RE_MAX_SPLIT_ID];
  RE_SPLIT_ID_TYPE splits_executed_count = 0;
  RE_SPLIT_ID_TYPE split_id, splits_executed_idx;

  int split_already_executed;

  RE_REPEAT_ARGS* repeat_args;
  RE_REPEAT_ANY_ARGS* repeat_any_args;

  RE_FIBER* fiber;
  RE_FIBER* last;
  RE_FIBER* prev;
  RE_FIBER* next;
  RE_FIBER* branch_a;
  RE_FIBER* branch_b;

  fiber = fiber_to_sync;
  prev = fiber_to_sync->prev;
  last = fiber_to_sync->next;

  while(fiber != last)
  {
    uint8_t opcode = *fiber->ip;

    switch(opcode)
    {
      case RE_OPCODE_SPLIT_A:
      case RE_OPCODE_SPLIT_B:

        split_id = *(RE_SPLIT_ID_TYPE*)(fiber->ip + 1);
        split_already_executed = FALSE;

        for (splits_executed_idx = 0;
             splits_executed_idx < splits_executed_count;
             splits_executed_idx++)
        {
          if (split_id == splits_executed[splits_executed_idx])
          {
            split_already_executed = TRUE;
            break;
          }
        }

        if (split_already_executed)
        {
          fiber = _yr_re_fiber_kill(fiber_list, fiber_pool, fiber);
        }
        else
        {
          branch_a = fiber;

          FAIL_ON_ERROR(_yr_re_fiber_split(
              fiber_list, fiber_pool, branch_a, &branch_b));


          if (opcode == RE_OPCODE_SPLIT_B)
            yr_swap(branch_a, branch_b, RE_FIBER*);


          branch_a->ip += (sizeof(RE_SPLIT_ID_TYPE) + 3);


          branch_b->ip += *(int16_t*)(
              branch_b->ip
              + 1  // opcode size
              + sizeof(RE_SPLIT_ID_TYPE));

          splits_executed[splits_executed_count] = split_id;
          splits_executed_count++;
        }

        break;

      case RE_OPCODE_REPEAT_START_GREEDY:
      case RE_OPCODE_REPEAT_START_UNGREEDY:

        repeat_args = (RE_REPEAT_ARGS*)(fiber->ip + 1);
        assert(repeat_args->max > 0);
        branch_a = fiber;

        if (repeat_args->min == 0)
        {
          FAIL_ON_ERROR(_yr_re_fiber_split(
              fiber_list, fiber_pool, branch_a, &branch_b));

          if (opcode == RE_OPCODE_REPEAT_START_UNGREEDY)
            yr_swap(branch_a, branch_b, RE_FIBER*);

          branch_b->ip += repeat_args->offset;
        }

        branch_a->stack[++branch_a->sp] = 0;
        branch_a->ip += (1 + sizeof(RE_REPEAT_ARGS));
        break;

      case RE_OPCODE_REPEAT_END_GREEDY:
      case RE_OPCODE_REPEAT_END_UNGREEDY:

        repeat_args = (RE_REPEAT_ARGS*)(fiber->ip + 1);
        fiber->stack[fiber->sp]++;

        if (fiber->stack[fiber->sp] < repeat_args->min)
        {
          fiber->ip += repeat_args->offset;
          break;
        }

        branch_a = fiber;

        if (fiber->stack[fiber->sp] < repeat_args->max)
        {
          FAIL_ON_ERROR(_yr_re_fiber_split(
              fiber_list, fiber_pool, branch_a, &branch_b));

          if (opcode == RE_OPCODE_REPEAT_END_GREEDY)
            yr_swap(branch_a, branch_b, RE_FIBER*);

          branch_a->sp--;
          branch_b->ip += repeat_args->offset;
        }

        branch_a->ip += (1 + sizeof(RE_REPEAT_ARGS));
        break;

      case RE_OPCODE_REPEAT_ANY_GREEDY:
      case RE_OPCODE_REPEAT_ANY_UNGREEDY:

        repeat_any_args = (RE_REPEAT_ANY_ARGS*)(fiber->ip + 1);


        if (fiber->rc == -1)
          fiber->rc = 0;

        if (fiber->rc < repeat_any_args->min)
        {

          fiber->rc++;
          fiber = fiber->next;
        }
        else if (fiber->rc < repeat_any_args->max)
        {

          next = fiber->next;
          branch_a = fiber;

          FAIL_ON_ERROR(_yr_re_fiber_split(
              fiber_list, fiber_pool, branch_a, &branch_b));

          if (opcode == RE_OPCODE_REPEAT_ANY_UNGREEDY)
            yr_swap(branch_a, branch_b, RE_FIBER*);

          branch_a->rc++;
          branch_b->ip += (1 + sizeof(RE_REPEAT_ANY_ARGS));
          branch_b->rc = -1;

          FAIL_ON_ERROR(_yr_re_fiber_sync(
              fiber_list, fiber_pool, branch_b));

          fiber = next;
        }
        else
        {

          fiber->ip += (1 + sizeof(RE_REPEAT_ANY_ARGS));
          fiber->rc = -1;
        }

        break;

      case RE_OPCODE_JUMP:
        fiber->ip += *(int16_t*)(fiber->ip + 1);
        break;

      default:
        if (_yr_re_fiber_exists(fiber_list, fiber, prev))
          fiber = _yr_re_fiber_kill(fiber_list, fiber_pool, fiber);
        else
          fiber = fiber->next;
    }
  }

  return ERROR_SUCCESS;
}
