#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define PRINT_DEBUG

static void print_reason(int sig)
{
    void *array[10];
    size_t size;
    size = backtrace(array, 10);
#ifdef PRINT_DEBUG
    char **strings;
    int i;
    strings = backtrace_symbols(array, size);
    printf("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    free(strings);

    char cmd[64] = "addr2line -C -f -e ";
    char* prog = cmd + strlen(cmd);
    readlink("/proc/self/exe", prog, sizeof(cmd) - strlen(cmd) - 1);// 获取进程的完整路径

    FILE* fp = popen(cmd, "w");
    if (fp != NULL)
    {
        for (i = 0; i < size; ++i)
        {
            fprintf(fp, "%p\n", array[i]);
        }
        pclose(fp);
    }
#else
    int fd = open("err.log", O_CREAT | O_WRONLY);
    backtrace_symbols_fd(array, size, fd);
    close(fd);
#endif
    exit(0);
}
void die()
{
    char *test1;
    char *test2;
    char *test3;
    char *test4 = NULL;
    strcpy(test4, "ab");
}
void test1()
{
    die();
}
int main(int argc, char **argv)
{
    struct sigaction myAction;
    myAction.sa_handler = print_reason;
    sigemptyset(&myAction.sa_mask);
    myAction.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SIGSEGV, &myAction, NULL); // 无效内存引用
    sigaction(SIGABRT, &myAction, NULL); // 异常终止
    test1();
}