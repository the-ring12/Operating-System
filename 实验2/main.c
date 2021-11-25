#include <stdio.h>
#include <unistd.h> // 进程
#include <signal.h> // 信号
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>   // 字符串和数字组合
#include <sys/wait.h> // 休眠

// 两个进程进程号，设置全局用于处理向子进程发送
int pid1;
int pid2;
int pipfd[2];

void main_handler(int signum)
{
    // 向子进程发送信号
    // 结束子进程1
    kill(pid1, SIGUSR1);
    wait(NULL);

    // 结束子进程1
    kill(pid2, SIGUSR2);
    wait(NULL);
    printf("Parent Process is killed!\n");
    sleep(2);
    exit(1);
}

void process1_handler(int signum)
{
    close(pipfd[1]);
    printf("\nChild Process 1 is killed by Parent!\n");
    sleep(2);
    exit(1);
}

void process2_handler(int signum)
{
    close(pipfd[0]);
    printf("\nChild Process 2 is killed by Parent!\n");
    sleep(2);
    exit(1);
}

int main()
{

    int pipeId = pipe(pipfd); // 创建管道
    if (pipeId == 0)
    {
        // 管道创建成功
        printf("管道创建成功\n");
    }
    else
    {
        printf("管道创建失败\n");
    }

    printf("设置终端信号\n");
    // 设置软中断信号 SIGINT 处理
    signal(SIGINT, main_handler);

    // 创建第一个子进程
    pid1 = fork();
    if (pid1 == 0)
    {
        // 子进程 1
        printf("进程1创建成功\n");

        // 忽略ctrl+c及处理 SIGUSR1
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR1, process1_handler);


        int i = 1;
        while (1) // 循环发送
        {
            // 设置进程间发送的内容
            char *start = "I send you ";
            char *end = " times";
            char msg[25] = "I send you x times";
            sprintf(msg, "%s%d%s", start, i++, end);
            // printf("%s\n", msg);

            // 向进程 2 发送消息
            close(pipfd[0]);
            write(pipfd[1], msg, strlen(msg) + 1);
            sleep(1);
        }
    }
    else if (pid1 < 0)
    {
        // 进程创建失败
        printf("进程1创建失败\n");
    }

    // 创建第二个子进程
    pid2 = fork();
    if (pid2 == 0)
    {
        // 子进程 2
        printf("进程2创建成功\n");

        // 忽略ctrl+c及处理 SIGUSR2
        signal(SIGINT, SIG_IGN);
        signal(SIGUSR2, process2_handler);

        while (1) // 循环接收消息
        {
            // 进程2通过通道接收消息
            char msg[25];
            memset(msg, '\0', sizeof(msg));
            close(pipfd[1]);
            ssize_t s = read(pipfd[0], msg, sizeof(msg));
            if (s > 0)
            {
                msg[s] = '\0';
            }
            printf("%s\n", msg);
        }
    }
    else if (pid2 < 0)
    {
        // 进程创建失败
        printf("进程2创建失败\n");
    }

    // printf("test");

    while (1)
    {
        // 让父进程持续运行
    }

    return 0;
}
