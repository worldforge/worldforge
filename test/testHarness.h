#ifndef ERIS_TEST_HARNESS
#define ERIS_TEST_HARNESS

#define ERIS_ASSERT(x) \
	if (!(x)) \
		printf("%s line %d: assertion %s failed\n", __FILE__, __LINE__, #x);
	
#define ERIS_ASSERT_MESSAGE(x, msg) \
	if (!(x)) \
		printf("%s line %d: assertion %s failed (%s)\n", __FILE__, __LINE__, #x, msg);

#define ERIS_MESSAGE(msg) \
	printf("%s line %d: %s\n", __FILE__, __LINE__, msg);

#endif
