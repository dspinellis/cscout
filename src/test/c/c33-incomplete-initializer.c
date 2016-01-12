typedef struct __wait_queue wait_queue_t;

struct list_head {
        struct list_head *next, *prev;
};

struct __wait_queue {
        unsigned int flags;
        void *private;
        int func;
        struct list_head task_list;
};


foo()
{
        wait_queue_t name = {
        .private        = "aa",
        .func           = 0,
	.task_list = {0, 0}
         };
        wait_queue_t *name2;

	name2->task_list.next = 0;
}
