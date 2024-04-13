void cgtimer_sub(cgtimer_t *a, cgtimer_t *b, cgtimer_t *res)
{
	res->QuadPart = a->QuadPart - b->QuadPart;
}
