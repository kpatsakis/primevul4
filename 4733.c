 */
static int wddx_stack_push(wddx_stack *stack, void *element, int size)
{
	if (stack->top >= stack->max) {		/* we need to allocate more memory */
		stack->elements = (void **) erealloc(stack->elements,
				   (sizeof(void **) * (stack->max += STACK_BLOCK_SIZE)));
	}
	stack->elements[stack->top] = (void *) emalloc(size);
	memcpy(stack->elements[stack->top], element, size);
	return stack->top++;