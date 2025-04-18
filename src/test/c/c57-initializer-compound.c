struct timespec64 {
	long tv_sec;
	int tv_nsec;
};


struct timespec64 
fuse_time_to_jiffies(long sec, int nsec) {
	struct timespec64 ts = {
		sec,
		//42,
		({ int  x = 42;   x; })
	};
	return ts;
}
