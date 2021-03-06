/*
 * 程序清单：保存与设置读取目录位置
 *
 * 程序会创建一个操作文件的线程
 * 在线程中调用 telldir() 函数
 * off_t telldir(DIR *d); 获取目录流的读取位置
 * void seekdir(DIR *d, off_t offset); 设置下次读取目录的位置
*/
#include <rtthread.h>
#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件 */

ALIGN(RT_ALIGN_SIZE)
static char dir_thread_stack[1024];
static struct rt_thread dir_thread;
/* 假设文件操作是在一个线程中完成 */
static void dir_thread_entry(void *parameter)
{
    DIR *dirp;
    int save3 = 0;
    int cur;
    int i = 0;
    struct dirent *dp;

    /* 打开根目录 */
    dirp = opendir("/");
    for (dp = readdir(dirp); dp != RT_NULL; dp = readdir(dirp))
    {
        /* 保存第三个目录项的目录指针*/
        if (i++ == 3)
            save3 = telldir(dirp);

        rt_kprintf("%s\n", dp->d_name);
    }

    /* 回到刚才保存的第三个目录项的目录指针*/
    seekdir(dirp, save3);

    /* 检查当前目录指针是否等于保存过的第三个目录项的指针. */
    cur = telldir(dirp);
    if (cur != save3)
    {
        rt_kprintf("seekdir (d, %ld); telldir (d) == %ld\n", save3, cur);
    }

    /* 从第三个目录项开始打印*/
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
        rt_kprintf("%s\n", dp->d_name);

    /* 关闭目录*/
    closedir(dirp);
}
int telldir_sample_init(void)
{
    rt_thread_init(&dir_thread,
                   "dir_thread",
                   dir_thread_entry,
                   RT_NULL,
                   &dir_thread_stack[0],
                   sizeof(dir_thread_stack), 8, 50);
    rt_thread_startup(&dir_thread);

    return 0;
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
INIT_APP_EXPORT(telldir_sample_init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(telldir_sample_init, telldir sample);
