#include "main.h"


//----------- the part about sync
#ifdef NATE_WITH_WORK_THREAD

static SDL_Mutex* s_mutex = NULL;
void s_CleanMutex()
{
    if (s_mutex) {
        SDL_DestroyMutex(s_mutex);
        s_mutex = NULL;
    }
}

static SDL_Thread* s_thread = NULL;
void s_CleanThread()
{
    if (s_thread) {
        SDL_WaitThread(s_thread, NULL);
        s_thread = NULL;
    }
}

bool s_EnsureMutex()
{
    if (!s_mutex)
    {
        s_mutex = SDL_CreateMutex();
        if (!s_mutex) {
            nate_SDL_GetError("SDL_CreateMutex(...)");
        }
    }
    return s_mutex != NULL;
}

static void s_JoinJoinableThread()
{
    if (s_thread) {
        SDL_ThreadState state = SDL_GetThreadState(s_thread);
        if (state == SDL_THREAD_COMPLETE) {
            SDL_WaitThread(s_thread, NULL);
            s_thread = NULL;
        }
    }
}

static int s_worker_function(void*);
// Please first init Mutex
bool s_EnsureRunningThread()
{
    s_JoinJoinableThread();
    if (!s_thread) {
        s_thread = SDL_CreateThread(s_worker_function, "Task", NULL);
        if (!s_thread) {
            nate_SDL_GetError("SDL_CreateThread(...)");
            s_CleanMutex();
        }
    }
    return s_thread != NULL;
}

bool nate_Task_Init()
{
    return s_EnsureMutex();
}

void nate_Task_Deinit()
{
    s_CleanMutex();
    s_CleanThread();
}

#else // NATE_WITH_WORK_THREAD

bool nate_Task_Init(){ return true; }
void nate_Task_Deinit(){}

#endif // !NATE_WITH_WORK_THREAD

// -------------- the part about collecting tasks
// 
static ArrayBuffer s_Tasks = ArrayBuffer0;
#define s_TasksData ((OneTask*)(s_Tasks.data))
typedef struct OneTask {
    nate_Task function;
    void* userdata;
} OneTask;

static bool s_add_task_sync(nate_Task function, void* userdata)
{
    if (s_Tasks.size >= s_Tasks.allocated) {
        int new_size = s_Tasks.size;
        if (new_size)
            new_size <<= 1;
        else
            new_size = 8;
        if (!nate_ArrayBuffer_Alloc(&s_Tasks, sizeof(OneTask), new_size)) {
            return false;
        }
    }
    
    s_TasksData[s_Tasks.size] = (OneTask){
        .function = function,
        .userdata = userdata
    };
    s_Tasks.size++;
    return true;
}

static OneTask s_fetch_task_sync()
{
    OneTask res = {0};
    if (s_Tasks.size > 0) {
        res = s_TasksData[0];
        s_Tasks.size--;
        for (size_t i = 0; i < s_Tasks.size; i++) {
            s_TasksData[i] = s_TasksData[i+1];
        }
    }
    return res;
}

#ifdef NATE_WITH_WORK_THREAD

bool nate_Task_Add(nate_Task function, void* userdata)
{
    SDL_LockMutex(s_mutex);
    bool res = s_add_task_sync(function, userdata);
    SDL_UnlockMutex(s_mutex);
    return res;
}

static int s_worker_function(void* _)
{
    OneTask curr_task;
    do
    {
        SDL_LockMutex(s_mutex);
        curr_task = s_fetch_task_sync();
        SDL_UnlockMutex(s_mutex);
        
        if (curr_task.function)
            curr_task.function(curr_task.userdata);
    }
    while(curr_task.function);
    return 0;
}

void nate_Task_Update() // Start/Join/Manage Threads and Tasks
{
    SDL_LockMutex(s_mutex);
    if (s_Tasks.size) {
        s_EnsureRunningThread();
    }
    else {
        s_JoinJoinableThread();
        if (s_Tasks.allocated) {
            nate_ArrayBuffer_Free(&s_Tasks);
        }
    }
    SDL_UnlockMutex(s_mutex);
}
#else // NATE_WITH_WORK_THREAD

bool nate_Task_Add(nate_Task function, void* userdata)
{
    return s_add_task_sync(function, userdata);
}

void nate_Task_Update() // Start/Join/Manage Threads and Tasks
{
    OneTask curr_task = s_fetch_task_sync();
    if (curr_task.function) {
        curr_task.function(curr_task.userdata);
    }    
}

#endif // !NATE_WITH_WORK_THREAD



