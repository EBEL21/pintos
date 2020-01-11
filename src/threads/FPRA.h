#define f (1<<14)

int FP_add(int x, int n) {
	return x+n*f;
}

int FP_sub(int x, int n) {
	return x-n*f;
}

int FP_mul_2float(int x, int y) {
	return ((int64_t)x)*y/f;
}

int FP_div_2float(int x, int y) {
	return ((int64_t)x)*f/y;
}

int convert_x_to_int_nearest(int x) {
	return x >= 0 ? (x+f/2)/f : (x-f/2)/f;
}

int convert_x_to_int_toward0(int x) {
	return x / f;	
}

int convert_n_to_fixed_point(int n) {
	return n*f;
}
