#include "vpu.c"
#include "hash.c"

void show(gap_buf *gap) {
	char buf[1024];
	printf("%lu [ ", gap->start);
	for(int i = 0; i < gap->start; ++i) {
		gap_item_to_cstr(buf, 1024, gap->buffer[i]);
		printf("%s ", buf);
	}
	printf("> %lu < ", gap->end - gap->start);
	for(int i = gap->end; i < gap->length; ++i) {
		gap_item_to_cstr(buf, 1024, gap->buffer[i]);
		printf("%s ", buf);
	}
	printf("] %lu\n", gap->length - gap->end);
}

hash_table *init_mnemonics() {
	const char *mnemonics[] = {
		"noop",
		"0",
		"1",
		"3",
		"9",
		"27",
		"81",
		"243",
		"+",
		"-",
		"*",
		"/%", // divmod
		">><<", // move gap by n
		"drop",
		"&>",
		"<&",
		"@",
	};
	hash_table *mnem = make_hash();
	for(int i = 0; i < 17; ++i)
		hash_put(mnem, mnemonics[i], i);
	return mnem;
}

int main() {
	char buf[8192] = "\0";
	gap_buf *gap = gap_buf_create(100);
	gap_vpu *vpu = gap_vpu_create(gap_vpu_builtins, gap_vpu_builtins, gap);
	hash_table *mnem = init_mnemonics();

	while(!feof(stdin)) {
		if(vpu->flags) {
			printf("VPU FLAGGED: %X\n", vpu->flags);
			vpu->flags = 0;
		}
		printf("%s ==>> ", buf);
		show(gap);
		buf[0] = '\0';
		scanf("%s", buf);
		int opid = hash_get(mnem, buf, -1);
		if(opid != -1) {
			vpu->builtins[opid](vpu);
		} else {
			printf("unrecognized operation: %s\n", buf);
		}
	}
	return 0;
}
