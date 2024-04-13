static double filter_box(double x) {
	if (x < - DEFAULT_FILTER_BOX)
		return 0.0f;
	if (x < DEFAULT_FILTER_BOX)
		return 1.0f;
	return 0.0f;
}
