#define snd_runtime_check(expr, args...) \
		snd_printk(KERN_ERR "ERROR (%s) (called from %p)\n", __ASTRING__(expr), __builtin_return_address(0));\
		args;

	snd_runtime_check(uinfo->type != SNDRV_CTL_ELEM_TYPE_BOOLEAN || uinfo->value.integer.min != 0 || uinfo->value.integer.max != 1, goto __unalloc);

