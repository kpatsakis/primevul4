arcStrokeCircle(const pointObj *p1, const pointObj *p2, const pointObj *p3,
                double segment_angle, int include_first, pointArrayObj *pa)
{
  pointObj center; /* Center of our circular arc */
  double radius; /* Radius of our circular arc */
  double sweep_angle_r; /* Total angular size of our circular arc in radians */
  double segment_angle_r; /* Segment angle in radians */
  double a1, /*a2,*/ a3; /* Angles represented by p1, p2, p3 relative to center */
  int side = arcSegmentSide(p1, p3, p2); /* What side of p1,p3 is the middle point? */
  int num_edges; /* How many edges we will be generating */
  double current_angle_r; /* What angle are we generating now (radians)? */
  int i; /* Counter */
  pointObj p; /* Temporary point */
  int is_closed = MS_FALSE;

  /* We need to know if we're dealing with a circle early */
  if ( FP_EQ(p1->x, p3->x) && FP_EQ(p1->y, p3->y) )
    is_closed = MS_TRUE;

  /* Check if the "arc" is actually straight */
  if ( ! is_closed && side == FP_COLINEAR ) {
    /* We just need to write in the end points */
    if ( include_first )
      pointArrayAddPoint(pa, p1);
    pointArrayAddPoint(pa, p3);
    return MS_SUCCESS;
  }

  /* We should always be able to find the center of a non-linear arc */
  if ( arcCircleCenter(p1, p2, p3, &center, &radius) == MS_FAILURE )
    return MS_FAILURE;

  /* Calculate the angles that our three points represent */
  a1 = atan2(p1->y - center.y, p1->x - center.x);
  /* UNUSED
  a2 = atan2(p2->y - center.y, p2->x - center.x);
   */
  a3 = atan2(p3->y - center.y, p3->x - center.x);
  segment_angle_r = M_PI * segment_angle / 180.0;

  /* Closed-circle case, we sweep the whole circle! */
  if ( is_closed ) {
    sweep_angle_r = 2.0 * M_PI;
  }
  /* Clockwise sweep direction */
  else if ( side == FP_LEFT ) {
    if ( a3 > a1 ) /* Wrapping past 180? */
      sweep_angle_r = a1 + (2.0 * M_PI - a3);
    else
      sweep_angle_r = a1 - a3;
  }
  /* Counter-clockwise sweep direction */
  else if ( side == FP_RIGHT ) {
    if ( a3 > a1 ) /* Wrapping past 180? */
      sweep_angle_r = a3 - a1;
    else
      sweep_angle_r = a3 + (2.0 * M_PI - a1);
  } else
    sweep_angle_r = 0.0;

  /* We don't have enough resolution, let's invert our strategy. */
  if ( (sweep_angle_r / segment_angle_r) < SEGMENT_MINPOINTS ) {
    segment_angle_r = sweep_angle_r / (SEGMENT_MINPOINTS + 1);
  }

  /* We don't have enough resolution to stroke this arc,
  *  so just join the start to the end. */
  if ( sweep_angle_r < segment_angle_r ) {
    if ( include_first )
      pointArrayAddPoint(pa, p1);
    pointArrayAddPoint(pa, p3);
    return MS_SUCCESS;
  }

  /* How many edges to generate (we add the final edge
  *  by sticking on the last point */
  num_edges = floor(sweep_angle_r / fabs(segment_angle_r));

  /* Go backwards (negative angular steps) if we are stroking clockwise */
  if ( side == FP_LEFT )
    segment_angle_r *= -1;

  /* What point should we start with? */
  if( include_first ) {
    current_angle_r = a1;
  } else {
    current_angle_r = a1 + segment_angle_r;
    num_edges--;
  }

  /* For each edge, increment or decrement by our segment angle */
  for( i = 0; i < num_edges; i++ ) {
    if (segment_angle_r > 0.0 && current_angle_r > M_PI)
      current_angle_r -= 2*M_PI;
    if (segment_angle_r < 0.0 && current_angle_r < -1*M_PI)
      current_angle_r -= 2*M_PI;
    p.x = center.x + radius*cos(current_angle_r);
    p.y = center.y + radius*sin(current_angle_r);
    pointArrayAddPoint(pa, &p);
    current_angle_r += segment_angle_r;
  }

  /* Add the last point */
  pointArrayAddPoint(pa, p3);
  return MS_SUCCESS;
}
