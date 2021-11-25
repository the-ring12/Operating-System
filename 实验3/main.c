#include <stdio.h>     // 标准输入输出
#include <string.h>    // 处理字符串
#include <semaphore.h> // 信号量
#include <sys/ipc.h>   // 共享内存
#include <sys/shm.h>   // 共享内存
#include <unistd.h>    // 进程
#include <signal.h>    // 信号
#include <wait.h>
#include <stdlib.h>
#include <fcntl.h>

#define NUM 5 // 共享内存块数

#define SEM_WRITE "semwrite"
#define SEM_READ "semread"
#define MUTEX "mutex"

// 信号量
sem_t *readsem, *writesem, *mutex;

// 两个进程
pid_t readbuffer;
pid_t writebuffer;
pid_t mianId; // 主进程

// 将共享内存连接成循环链表
typedef struct Node
{
    int shmid;
    struct Node *next;
} node, *ptrNode;

key_t KEY;

// 共享内存头指针
ptrNode head;

// 读写节点
ptrNode writeNode, readNode;

// 文件
FILE *fp, *fp2;

void createShm();                          // 创建共享内存环
void destroyShm();                         // 销毁共享内环
void initSem();                            // 初始化信号量
void destroySem();                         // 销毁信号量
void mainHandler(int signum);              // 主进程处理信号
void readHandler(int signum);              // readbuffer进程处理信号
void sem_file(); // readbuffer 从共享内存写入文件

void writeprocess(); // writebuffer 执行
void readprocess();  // readbuffer 执行

// 主函数
int main()
{

    KEY = ftok("tan", 2021);

    // 开启信号
    mianId = getpid();
    signal(SIGINT, mainHandler);

    // 创建共享内存环
    createShm();

    // 初始化信号量
    initSem();

    // 初始化读写节点
    readNode = writeNode = head;

    // 创建 readbuffer 进程
    readbuffer = fork();
    if (readbuffer < 0)
    {
        printf("writebuffer 进程创建失败！！！\n");
    }
    if (readbuffer == 0)
    {
        // 执行读取文件写入共享内存
        readprocess();
    }

    // 创建 readbuffer 进程
    writebuffer = fork();
    if (writebuffer < 0)
    {
        printf("writebuffer 进程创建失败！！！\n");
    }
    if (writebuffer == 0)
    {
        // 执行读取共享内存信息写入文件
        writeprocess();
    }

    while (1)
    {
        sleep(3);
        int i = 0;
        sem_getvalue(writesem, &i);
        printf("main:writesem: %d\n", i);
        sem_getvalue(readsem, &i);
        printf("main:readsem: %d\n", i);
    }

    return 0;
}

void writeprocess()
{
    // 初始化写节点
    writeNode = head;
    char ch[64];
    int shmid;

    // 打开文件
    if ((fp = fopen("test1", "r")) == NULL)
    {
        printf("打开 test1 失败 ！！！\n");
    }

    // 读文件
    while (!feof(fp))
    {
        // 绑定共享内存块
        shmid = writeNode->shmid;

        printf("write:%d\n", shmid);

        // P操作
        sem_wait(writesem);
        sem_wait(mutex);
        printf("write\n");

        // 绑定共享内存
        char *shmadd = shmat(shmid, NULL, 0);
        if (shmadd < 0)
        {
            printf("绑定共享内存失败!!!\n");
        }

        // 读取文件
        fgets(ch, 64, fp);
        printf("write:%s", ch);

        // 写入共享内存
        strcpy(shmadd, ch);

        // 解除绑定
        if (shmdt(shmadd) < 0)
        {
            printf("接触绑定失败!!\n");
        }

        // V操作
        sem_post(readsem);
        sem_post(mutex);

        // 调试：查看信号灯的值
        int i = 0;
        sem_getvalue(writesem, &i);
        printf("write:writesem: %d\n", i);
        sem_getvalue(readsem, &i);
        printf("write:readsem: %d\n", i);
        sem_getvalue(mutex, &i);
        printf("write:mutex: %d\n", i);

        // 共享内存中写的节点移至下一个节点
        writeNode = writeNode->next;
    }

    // 向主进程和readbuffer发送信号结束
    kill(readbuffer, SIGINT);
    kill(mianId, SIGINT);
    exit(1);
}

void readprocess()
{

    // 信号处理
    signal(SIGINT, readHandler);

    // 初始化写节点
    // readNode = head;

    // 打开文件
    if ((fp2 = fopen("test2", "a")) == NULL)
    {
        printf("打开 test2 失败 ！！！\n");
    }

    // 读共享内存，写文件
    while (1)
    {
        printf("while:%d\n", readNode->shmid);
        sem_file();
    }
}
int count = 1;
void sem_file()
{
    printf("%d\n", count++);
    printf("%d\n", readNode->shmid);
    char ch[64];
    // 绑定共享内存块
    int shmid = readNode->shmid;
    printf("read:%d\n", shmid);

    // 调试：查看信号灯的值
    int i = 0;
    sem_getvalue(writesem, &i);
    printf("read:writesem: %d\n", i);
    sem_getvalue(readsem, &i);
    printf("read:readsem: %d\n", i);
    sem_getvalue(mutex, &i);
    printf("read:mutex: %d\n", i);

    // P操作
    printf("P before:read\n");
    sem_wait(readsem);
    sem_wait(mutex);
    printf("P after:read\n");

    // 绑定共享内存
    char *shmadd = shmat(shmid, NULL, 0);
    if (shmadd < 0)
    {
        printf("绑定共享内存失败!!!\n");
    }

    /* 查看共享内存是否有值
           这一步应该是多余的，因为使用了信号灯进行控制
           且，这时一个环形的共享内存区域，不使用信号灯还会出现重复写的情况
           */
    if (shmadd == NULL)
    {
        printf("值为空！！\n");
    }
    // printf("read:%s\n", shmadd);

    // 读取共享内存数据
    strcpy(ch, shmadd);

    printf("read:%s\n", ch);

    // 写入文件按
    fputs(ch, fp2);

    // 解除绑定
    if (shmdt(shmadd) < 0)
    {
        printf("解除绑定失败!!\n");
    }

    // V操作
    sem_post(writesem);
    sem_post(mutex);

    // 读节点移动到下一节点
    readNode = readNode->next;
}

// 创建环形共享内存区
void createShm()
{
    int shmid;
    // 创建或打开一块共享内存区
    head = (ptrNode)malloc(sizeof(node));
    shmid = shmget(KEY, 64, IPC_CREAT);
    if (shmid < 0)
    {
        printf("共享内存创建失败!!!\n");
        return;
    }

    head->shmid = shmid;
    ptrNode cur = (ptrNode)malloc(sizeof(node));
    head->next = cur;

    // 创建NUM个共享内存，并连接成环
    int i;
    for (i = 1; i < NUM - 1; i++)
    {
        shmid = shmget(KEY + i, 64, IPC_CREAT);
        if (shmid < 0)
        {
            printf("共享内存创建失败!!!\n");
            return;
        }
        cur->shmid = shmid;
        ptrNode tmp = (ptrNode)malloc(sizeof(node));
        cur->next = tmp;
        cur = tmp;
    }
    shmid = shmget(KEY + NUM, 64, IPC_CREAT);
    if (shmid < 0)
    {
        printf("共享内存创建失败!!!\n");
        return;
    }
    cur->shmid = shmid;
    cur->next = head;
}

// 销毁环形共享内存区域
void destroyShm()
{
    ptrNode cur = head;

    while (cur->next != head)
    {
        // 销毁共享内存
        shmctl(cur->shmid, IPC_RMID, NULL);
        ptrNode tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    shmctl(cur->shmid, IPC_RMID, NULL);
    free(cur);
}

// 初始化信号灯
void initSem()
{
    writesem = sem_open(SEM_WRITE, O_CREAT, 0644, NUM);
    readsem = sem_open(SEM_READ, O_CREAT, 0644, 0);
    mutex = sem_open(MUTEX, O_CREAT, 0644, 1);
}

// 关闭并销毁信号灯
void destroySem()
{
    // while (sem_close(writesem) < 0)
    // {
    //     printf("writesem 销毁失败！！\n");
    // }
    // while (sem_close(readsem) < 0)
    // {
    //     printf("readsem 销毁失败！！\n");
    // }
    // while (sem_close(mutex) < 0)
    // {
    //     printf("mutexsem 销毁失败！！\n");
    // }

    printf("删除信号量成功！\n");
    if (sem_unlink(SEM_WRITE) < 0)
    {
        sem_close(writesem);
        printf("write 销毁失败！！\n");
    }
    if (sem_unlink(SEM_READ) < 0)
    {
        sem_close(readsem);
        printf("read 销毁失败！！\n");
    }
    if (sem_unlink(MUTEX) < 0)
    {
        sem_close(mutex);
        printf("mutex 销毁失败！！\n");
    }
}

// 主函数接收到 SIGINT 的处理函数
void mainHandler(int signum)
{
    // 等待子进程结束
    wait(NULL);
    wait(NULL);
    destroyShm();
    destroySem();
    exit(1);
}

// readbuffer 接收到 SIGINT 的处理函数
void readHandler(int signum)
{
    printf("%d\n", signum);
    int i;
    sem_getvalue(readsem, &i);
    while (i > 0)
    {
        printf("readhandler: %d\n", i);
        sem_file();
        sem_getvalue(readsem, &i);
    }
    exit(1);
}