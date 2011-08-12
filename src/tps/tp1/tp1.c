int main(int argc, char ** params) {
	return sim_frontend_main(
		params_to_list(argc, params)
	);
}

list params_to_list(int len, char ** params) {
	list l = list_init();
	int i = 0;
	for (i = 0; i < len; i++) {
		list_add(l, params[i]);
	}
	return l;
}
