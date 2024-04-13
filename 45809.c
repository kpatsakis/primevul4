arcCircleCenter(const pointObj *p1, const pointObj *p2, const pointObj *p3, pointObj *center, double *radius)
{
  pointObj c;
  double r;

  /* Components of the matrices. */
  double x1sq = p1->x * p1->x;
  double x2sq = p2->x * p2->x;
  double x3sq = p3->x * p3->x;
  double y1sq = p1->y * p1->y;
  double y2sq = p2->y * p2->y;
  double y3sq = p3->y * p3->y;
  double matrix_num_x[9];
  double matrix_num_y[9];
  double matrix_denom[9];

  /* Intialize matrix_num_x */
  matrix_num_x[0] = x1sq+y1sq;
  matrix_num_x[1] = p1->y;
  matrix_num_x[2] = 1.0;
  matrix_num_x[3] = x2sq+y2sq;
  matrix_num_x[4] = p2->y;
  matrix_num_x[5] = 1.0;
  matrix_num_x[6] = x3sq+y3sq;
  matrix_num_x[7] = p3->y;
  matrix_num_x[8] = 1.0;

  /* Intialize matrix_num_y */
  matrix_num_y[0] = p1->x;
  matrix_num_y[1] = x1sq+y1sq;
  matrix_num_y[2] = 1.0;
  matrix_num_y[3] = p2->x;
  matrix_num_y[4] = x2sq+y2sq;
  matrix_num_y[5] = 1.0;
  matrix_num_y[6] = p3->x;
  matrix_num_y[7] = x3sq+y3sq;
  matrix_num_y[8] = 1.0;

  /* Intialize matrix_denom */
  matrix_denom[0] = p1->x;
  matrix_denom[1] = p1->y;
  matrix_denom[2] = 1.0;
  matrix_denom[3] = p2->x;
  matrix_denom[4] = p2->y;
  matrix_denom[5] = 1.0;
  matrix_denom[6] = p3->x;
  matrix_denom[7] = p3->y;
  matrix_denom[8] = 1.0;

  /* Circle is closed, so p2 must be opposite p1 & p3. */
  if ( FP_EQ(p1->x,p3->x) && FP_EQ(p1->y,p3->y) ) {
    c.x = (p1->x + p2->x) / 2.0;
    c.y = (p1->y + p2->y) / 2.0;
    r = sqrt( (p1->x - p2->x) * (p1->x - p2->x) + (p1->y - p2->y) * (p1->y - p2->y) ) / 2.0;
  }
  /* There is no circle here, the points are actually co-linear */
  else if ( arcSegmentSide(p1, p3, p2) == FP_COLINEAR ) {
    return MS_FAILURE;
  }
  /* Calculate the center and radius. */
  else {
    double denom = 2.0 * arcDeterminant3x3(matrix_denom);
    /* Center components */
    c.x = arcDeterminant3x3(matrix_num_x) / denom;
    c.y = arcDeterminant3x3(matrix_num_y) / denom;

    /* Radius */
    r = sqrt((p1->x-c.x) * (p1->x-c.x) + (p1->y-c.y) * (p1->y-c.y));
  }

  if ( radius ) *radius = r;
  if ( center ) *center = c;

  return MS_SUCCESS;
}
