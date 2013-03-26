#include <stdlib.h>
#include <strings.h>
#include <stdint.h>
#include <limits.h>

enum uint8_t {
	gi_signed,
	gi_unsigned,
	gi_double,
	gi_pointer,
} gap_item_types;

typedef struct gap_item {
	union {
		struct {
			uint8_t type;
		} info;
		uint64_t _tag;
	} tag;
	union {
		int64_t _signed;
		uint64_t _unsigned;
		double _double;
		void *_pointer;
	} value;
} gap_item;

gap_item gap_item_from_int(int value) {
	gap_item item;
	item.tag.info.type = gi_signed;
	item.value._signed = value;
	return item;
}

int gap_item_to_int(gap_item item) {
	switch(item.tag.info.type) {
		case gi_signed:
			return (int) item.value._signed;
		case gi_unsigned:
			return (int) item.value._unsigned;
		case gi_double:
			return (int) item.value._double;
		case gi_pointer:
			return (int) item.value._pointer;
		default:
			return INT_MIN;
	}
}

typedef struct gap_buf {
	gap_item *buffer;
	size_t length;
	size_t start; // start is the first "empty" part of the gap, inclusive
	size_t end;  // end is the last "empty" part of the gap, exclusive
} gap_buf;

gap_buf *gap_buf_create(size_t length) {
	gap_buf *buf = calloc(sizeof(gap_buf), 1);
	buf->buffer = calloc(sizeof(gap_item), length);
	buf->length = length;
	buf->start = 0;
	buf->end = length;
	return buf;
}

int gap_cannot(gap_buf *gap, int minl, int ming, int minr) {
	int status = 0;
	status |= (gap->start < minl) ? 1 : 0;
	status |= (gap->end - gap->start < ming) ? 2 : 0;
	status |= (gap->length - gap->end < minr) ? 4 : 0;
	return status;
}

void pushl_gap_buf(gap_buf *buf, gap_item val) {
	buf->buffer[(buf->start)++] = val;
}

gap_item popl_gap_buf(gap_buf *buf) {
	return buf->buffer[--(buf->start)];
}

void pushr_gap_buf(gap_buf *buf, gap_item val) {
	buf->buffer[--(buf->end)] = val;
}

gap_item popr_gap_buf(gap_buf *buf) {
	return buf->buffer[(buf->end)++];
}

struct gap_vpu;

typedef void (*gap_vpu_op)(struct gap_vpu *);

typedef struct gap_vpu {
	gap_buf *gap;
	int flags;
	gap_vpu_op builtins[128];
	gap_vpu_op userops[128];
} gap_vpu;

gap_vpu *gap_vpu_create(const gap_vpu_op builtins[128], const gap_vpu_op userops[128], gap_buf *gap) {
	gap_vpu *vpu = calloc(sizeof(gap_vpu), 1);
	vpu->gap = gap;
	memmove(vpu->builtins, builtins, sizeof(gap_vpu_op) * 128);
	memmove(vpu->userops, userops, sizeof(gap_vpu_op) * 128);
	return vpu;
}

#define VPU_REQUIRE_GAP(vpu, minl, ming, minr) \
	{ \
		int __VPU_REQUIRE_GAP_status = gap_cannot((vpu)->gap, (minl), (ming), (minr)); \
		if ( __VPU_REQUIRE_GAP_status ) { \
			vpu->flags = __VPU_REQUIRE_GAP_status; \
			return; \
		} \
	}

void gap_bi_noop(gap_vpu *vpu) {
	return;
}

void gap_bi_push0(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(0));
}

void gap_bi_push1(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(1));
}

void gap_bi_push3(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(3));
}

void gap_bi_push9(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(9));
}

void gap_bi_push27(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(27));
}

void gap_bi_push81(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(81));
}

void gap_bi_push243(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 0, 1, 0);
	pushl_gap_buf(vpu->gap, gap_item_from_int(243));
}

void gap_bi_add(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 2, 0, 0);
	gap_item a = popl_gap_buf(vpu->gap);
	gap_item b = popl_gap_buf(vpu->gap);
	int c = gap_item_to_int(b) + gap_item_to_int(a);
	pushl_gap_buf(vpu->gap, gap_item_from_int(c));
}

void gap_bi_sub(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 2, 0, 0);
	gap_item a = popl_gap_buf(vpu->gap);
	gap_item b = popl_gap_buf(vpu->gap);
	int c = gap_item_to_int(b) - gap_item_to_int(a);
	pushl_gap_buf(vpu->gap, gap_item_from_int(c));
}

void gap_bi_mul(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 2, 0, 0);
	gap_item a = popl_gap_buf(vpu->gap);
	gap_item b = popl_gap_buf(vpu->gap);
	int c = gap_item_to_int(b) * gap_item_to_int(a);
	pushl_gap_buf(vpu->gap, gap_item_from_int(c));
}

void gap_bi_divmod(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 2, 0, 0);
	gap_item a = popl_gap_buf(vpu->gap);
	gap_item b = popl_gap_buf(vpu->gap);
	int a_ = gap_item_to_int(a);
	int b_ = gap_item_to_int(b);
	if(a_ == 0) {
		vpu->flags = 8;
		pushl_gap_buf(vpu->gap, gap_item_from_int(0));
		pushl_gap_buf(vpu->gap, gap_item_from_int(0));
		return;
	}
	int c = b_ / a_;
	int d = b_ % a_;
	pushl_gap_buf(vpu->gap, gap_item_from_int(c));
	pushl_gap_buf(vpu->gap, gap_item_from_int(d));
}

void gap_bi_warp_rel(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 1, 0, 0);
	int warp = gap_item_to_int(popl_gap_buf(vpu->gap));
	gap_item (*pop)(gap_buf*);
	void (*push)(gap_buf *, gap_item);
	if(warp < 0) {
		warp = -warp;
		VPU_REQUIRE_GAP(vpu, warp, 0, 0);
		pop = popl_gap_buf;
		push = pushr_gap_buf;
	} else {
		VPU_REQUIRE_GAP(vpu, 0, 0, warp);
		pop = popr_gap_buf;
		push = pushl_gap_buf;
	}
	while(warp--) {
		push(vpu->gap, pop(vpu->gap));
	}
}

void gap_bi_drop(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 1, 0, 0);
	popl_gap_buf(vpu->gap);
}

void gap_bi_pushrefl(gap_vpu *vpu) {
	VPU_REQUIRE_GAP(vpu, 1, 1, 0);
	gap_item item;
	item.info.type = gi_pointer;
	item.value._pointer = &(vpu->gap->base[vpu->gap->start - 1]);
	pushl_gap_buf(vpu->gap, item);
}

const gap_vpu_op gap_vpu_builtins[128] = {
	gap_bi_noop,

	gap_bi_push0,
	gap_bi_push1,
	gap_bi_push3,
	gap_bi_push9,
	gap_bi_push27,
	gap_bi_push81,
	gap_bi_push243,

	gap_bi_add,
	gap_bi_sub,
	gap_bi_mul,
	gap_bi_divmod,

	gap_bi_warp_rel,
	gap_bi_drop,
};
