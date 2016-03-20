/*Queue - Linked List implementation*/
#include <mach/mach_time.h>
#include <stdint.h>

#include<stdio.h>
#include<stdlib.h>

#define majority_printf(fmt, args...)   //printf(fmt, ##args);

struct Node {
	uint32_t data;
    uint64_t ts; //timestamp
	struct Node* next;
};

#if 0
typedef struct {
    struct Node* front;
    struct Node* rear;
    int len;
    int max;
} Queue;

void Initqueue(Queue* q, int max);
void Dequeue(Queue* q);
void Enqueue(Queue* q, uint32_t x);
int Front(Queue* q);
void Print(Queue* q);
void DequeueByTimestamp(Queue* q, uint32_t diff);
int Majority(Queue* q);

void Initqueue(Queue* q, int max) {
    q->front = q->rear = NULL;
    q->len = 0;
    q->max = max;
}

// To Enqueue an integer
void Enqueue(Queue* q, uint32_t x) {
    printf("Push: %d\n", x);
    if(q->len+1 >= q->max)
    {
        Dequeue(q);
    }
	struct Node* temp = 
		(struct Node*)malloc(sizeof(struct Node));
	temp->data = x; 
    temp->ts = mach_absolute_time();
	temp->next = NULL;
	if(q->front == NULL && q->rear == NULL){
		q->front = q->rear = temp;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
    q->len++;
}

// To Dequeue an integer.
void Dequeue(Queue* q) {
	struct Node* temp = q->front;
	if(q->front == NULL) {
		printf("Queue is Empty\n");
		return;
	}
	if(q->front == q->rear) {
		q->front = q->rear = NULL;
	}
	else {
		q->front = q->front->next;
	}
    printf("Del: %d\n", temp->data);
	free(temp);
    q->len--;
}

int Front(Queue* q) {
	if(q->front == NULL) {
		printf("Queue is empty\n");
		return -1;
	}
	return q->front->data;
}

void Print(Queue* q) {

	struct Node* temp = q->front;
	while(temp != NULL) {
		printf("%d(%lld) ",temp->data, temp->ts);
		temp = temp->next;
	}
	printf("\n");
}


void DequeueByTimestamp(Queue* q, uint32_t diff) {
    uint64_t current_ts = mach_absolute_time();
    printf("current: %lld\n", current_ts);
	struct Node* temp = q->front;
	while(temp != NULL) {
        if(current_ts - temp->ts > diff)
        {
            
		    printf("Del: %d(%lld)[%lld]\n",temp->data, temp->ts, current_ts-temp->ts);
            Dequeue(q);
        }
        else
        {
		    printf("Safe: %d(%lld)[%lld]\n",temp->data, temp->ts, current_ts-temp->ts);
        }
		temp = temp->next;
	}
	printf("\n");
}

#define MAX 10
int Majority(Queue* q) {
    int a = 0; //Majority
    int m = 0;
    int i = 0;
    int used = 0;
    int found = -1;
    int map[MAX][2] = {0};
    
	struct Node* temp = q->front;
	while(temp != NULL) {
        found = -1;
        for(i=0; i<used; i++)
        {
            if(temp->data==map[i][0])
            {
                found = i;
            }
        }
        if(found == -1)
        {
            map[used][0] = temp->data;
            map[used++][1] = 1;

        }
        else
        {
            map[found][1]++;
        }
		temp = temp->next;
	}    
    for(i=0; map[i][1]; i++)
    {
        if(map[i][1] > m)
        {
            a = map[i][0];
            m = map[i][1];
        }   
    }
    return a;
}
#endif
//==========================================================
//==========================================================
//==========================================================

typedef struct {
    struct Node* front;
    struct Node* rear;
    int len;
    int max;
} SlidingWindow;

#define SLIDINGWINDOW_MAX      2
#define SLIDINGWINDOW_EL_MAX   10
#define SLIDINGWINDOW_FD_CNR   0
#define SLIDINGWINDOW_FD_SS    1  //Signal Strength
#define SLIDINGWINDOW_TS_DIFF  1000000 //1ms

#define check_sliding_window_fd(fd) {\
    if(fd>=SLIDINGWINDOW_MAX)\
    {\
        printf("[%s] ERR: Incorrect fd number(%d)\n", __FUNCTION__, fd);\
        exit(1);\
    }else{_InitSlidingWindow(fd);}}\

static SlidingWindow* stSlidingWindowList[SLIDINGWINDOW_MAX];

void _InitSlidingWindow(int fd)
{
    if(NULL == stSlidingWindowList[fd])
    {
        printf("%s: fd-%d\n", __FUNCTION__, fd);
        stSlidingWindowList[fd] = (SlidingWindow*)malloc(sizeof(SlidingWindow));
        stSlidingWindowList[fd]->front = stSlidingWindowList[fd]->rear = NULL;
        stSlidingWindowList[fd]->len = 0;
        stSlidingWindowList[fd]->max = SLIDINGWINDOW_EL_MAX;
    }
}

void _DelSlidingWindow(int fd)
{
    check_sliding_window_fd(fd);
    SlidingWindow* sw = stSlidingWindowList[fd];
    struct Node* temp = sw->front;
    if(sw->front == NULL) {
        printf("SlidingWindow is Empty\n");
        return;
    }
    if(sw->front == sw->rear) {
        sw->front = sw->rear = NULL;
    }
    else {
        sw->front = sw->front->next;
    }
    printf("Del: %d\n", temp->data);
    free(temp);
    sw->len--;
}

void _AddSlidingWindow(int fd, int x)
{
    check_sliding_window_fd(fd);  
    SlidingWindow* sw = stSlidingWindowList[fd];
    uint64_t current_ts = mach_absolute_time(); ////get system time
    struct Node* temp;
    
    /* Remove old data */
    temp = sw->front;
    while(temp != NULL) {
        if(current_ts - temp->ts > SLIDINGWINDOW_TS_DIFF)
        {
            printf("Del: %d(%lld)[%lld]\n",temp->data, temp->ts, current_ts-temp->ts);
            _DelSlidingWindow(fd);
        }
        else
        {
            printf("Safe: %d(%lld)[%lld]\n",temp->data, temp->ts, current_ts-temp->ts);
        }
        temp = temp->next;
    }
    temp = NULL;
     
    /* Remove one element for new data */
    if(sw->len+1 >= sw->max)
    {
        _DelSlidingWindow(fd);
    }

    temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = x; 
    temp->ts = mach_absolute_time();  //get system time
    temp->next = NULL;
    if(sw->front == NULL && sw->rear == NULL){
        sw->front = sw->rear = temp;
        return;
    }
    sw->rear->next = temp;
    sw->rear = temp;
    sw->len++;
}

int _GetMajoritySlidingWindow(int fd)
{
    check_sliding_window_fd(fd);  
    int a = -1; //Majority
    int m = 0;
    int i = 0;
    int used = 0;
    int found = -1;
    int map[SLIDINGWINDOW_EL_MAX][2] = {0};
    SlidingWindow* sw = stSlidingWindowList[fd];
    struct Node* temp = sw->front;
    while(temp != NULL) {
        found = -1;
        for(i=0; i<used; i++)
        {
            if(temp->data==map[i][0])
            {
                found = i;
            }
        }
        if(found == -1)
        {
            map[used][0] = temp->data;
            map[used++][1] = 1;
        }
        else
        {
            map[found][1]++;
        }
        temp = temp->next;
    }
    for(i=0; map[i][1]; i++)
    {
        if(map[i][1] > m)
        {
            a = map[i][0];
            m = map[i][1];
        }
    }
    return a;
}

void _PrintSlidingWindow(int fd)
{
    check_sliding_window_fd(fd);
    printf("%s data: ", __FUNCTION__);
    SlidingWindow* sw = stSlidingWindowList[fd];
    struct Node* temp = sw->front;
    while(temp != NULL) {
        printf("%d, ",temp->data);
        temp = temp->next;
    }
    printf("\n");
}


int main(){
    /*
    _InitSlidingWindow(SLIDINGWINDOW_FD_CNR);
    _InitSlidingWindow(SLIDINGWINDOW_FD_SS);
    */
    
    _AddSlidingWindow(SLIDINGWINDOW_FD_CNR, 20);
    _AddSlidingWindow(SLIDINGWINDOW_FD_CNR, 202);
    _AddSlidingWindow(SLIDINGWINDOW_FD_CNR, 20);
    _AddSlidingWindow(SLIDINGWINDOW_FD_CNR, 210);

    int a = _GetMajoritySlidingWindow(SLIDINGWINDOW_FD_CNR);
    printf("%s: %d\n", __FUNCTION__, a);
    _PrintSlidingWindow(SLIDINGWINDOW_FD_CNR);
}

