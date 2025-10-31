// Both-begin
int both;

#ifdef PRJ2
// Only PRJ2
int prj2;
#endif

// Both-middle

#if !defined(PRJ2)
// Only prj1
int prj1;
#endif

// Both-end
