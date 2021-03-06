#ifndef TEST_H
#define TEST_H
/*
 * Test.h -- basic test framework
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define __func__ __FUNCTION__
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEST_NAME
#define TEST_NAME __FILE__
#endif

// NOTE the token-paste operator (##) behaves specially with __VA_ARGS__ after commas
#define TEST_LOG(fmt, ...) printf("%s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define TEST_CALL(testfn) test((testfn_t) testfn, #testfn)
#define TESTARG(testfn) (testfn),(#testfn)
#define TEST_RUN(...) \
        testfn_t test_functions[] = {__VA_ARGS__}; \
        char test_names[] = #__VA_ARGS__; \
        int size = sizeof(test_functions) / sizeof(void *); \
        int names[size]; \
        test_names_parse(test_functions, test_names, names, size); \
        unsigned u; \
        int ret = 0; \
        for (u = 0; u < sizeof(test_functions) / sizeof(void *); u++) { \
            ret += test_time(test_functions[u], (const char *) &test_names[names[u]]); \
        } \
        return test_eval(TEST_NAME, ret); \

#ifdef TEST_FILE
#define TEST_MAIN(...) \
    int main(int, char **) { \
        TEST_RUN(__VA_ARGS__); \
    }
#undef TEST_FILE
#else
#define TEST_MAIN(...)
#endif

#define TEST_DEFINE(testfn) extern "C" int testfn()
#define TEST_DECLARE(testfn) extern "C" int testfn()
#define TEST_ABORT() ASSERT(false)
#define TEST_FAIL() do {TEST_LOG("FAILURE!"); TEST_ABORT();} while(0)
#define TEST_ASSERT(expr) do {if (!(expr)) TEST_FAIL();} while (0)

#define TEST_SUCCESS 0
#define TEST_FAILURE 1

typedef int (*testfn_t)();

int test(testfn_t testfn, const char *testname);
int test_iter(testfn_t testfn, const char *testname, unsigned long niter);
int test_time(testfn_t testfn, const char *testname);
int test_iter_time(testfn_t testfn, const char *testname, unsigned long niter);
int test_eval(const char *ident, int result);
int test_system(const char *cmdline);

// helper function for TEST_MAIN
int test_names_parse(testfn_t *test_functions, char *test_names, int *names, int size);

#ifdef __cplusplus
}
#endif

#endif
