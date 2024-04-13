IW_IMPL(int) iw_is_valid_density(double density_x, double density_y, int density_code)
{
	if(density_x<0.0001 || density_y<0.0001) return 0;
	if(density_x>10000000.0 || density_y>10000000.0) return 0;
	if(density_x/10.0>density_y) return 0;
	if(density_y/10.0>density_x) return 0;
	if(density_code!=IW_DENSITY_UNITS_UNKNOWN && density_code!=IW_DENSITY_UNITS_PER_METER)
		return 0;
	return 1;
}
