// Inspired by linux/drivers/block/drbd/drbd_polymorph_printk.h

#define x 1, 2
#define add_param() __builtin_choose_expr(x, 42)

int i = add_param();

struct s {
	// Bitfield width
	int i : add_param();
};

void
foo(unsigned long size, unsigned long offset)
{

#define _UL(x) (x)
#define __BITS_PER_LONG 64
#define __is_constexpr(x) \
        (sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))

#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int:(-!!(e)); })))

#define GENMASK_INPUT_CHECK(h, l) \
(BUILD_BUG_ON_ZERO(__builtin_choose_expr( \
	__is_constexpr((l) > (h)), (l) > (h), 0)))

#define __GENMASK(h, l) \
        (((~_UL(0)) - (_UL(1) << (l)) + 1) & \
         (~_UL(0) >> (__BITS_PER_LONG - 1 - (h))))

#define GENMASK(h, l) \
        (GENMASK_INPUT_CHECK(h, l) + __GENMASK(h, l))

int val = GENMASK(size - 1, offset);

}

struct fred_cs {
int cs : 16,
: 45;
};

