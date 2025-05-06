#include "queue.h"

// 初始化队列
void Point_Queue_Init(Point_Queue *q) {
    q->data->x = 65535;
    q->data->y = 65535;
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

// 检查空队列
bool Point_Queue_IsEmpty(Point_Queue *q) {
    return (q->size == 0);
}

// 检查满队列
bool Point_Queue_IsFull(Point_Queue *q) {
    return (q->size == TARGET_QUEUE_SIZE);
}

// 入队操作
void Point_Queue_Enqueue(Point_Queue *q, Pixel_Point point) {
    if (Point_Queue_IsFull(q)) {
        return;  // 可改为返回错误码
    }
    q->rear = (q->rear + 1) % TARGET_QUEUE_SIZE;
    q->data[q->rear] = point;
    q->size++;
}

// 出队操作
Pixel_Point Point_Queue_Dequeue(Point_Queue *q) {
    Pixel_Point empty_point = {65535, 65535};  // 默认返回值
    if (Point_Queue_IsEmpty(q)) {
        return empty_point;
    }
    Pixel_Point val = q->data[q->front];
    q->front = (q->front + 1) % TARGET_QUEUE_SIZE;
    q->size--;
    return val;
}

// 查看队首
Pixel_Point Point_Queue_Peek(Point_Queue *q) {
    Pixel_Point empty_point = {65535, 65535};
    return Point_Queue_IsEmpty(q) ? empty_point : q->data[q->front];
}

//双倍队列，每个点变为两个点
void Point_Queue_Double(Point_Queue *q)
{
    for(uint16_t Size = q->size; Size > 0; Size --)
    {
        Pixel_Point p = Point_Queue_Dequeue(q);
        Point_Queue_Enqueue(q,p);
        Point_Queue_Enqueue(q,p);
    }
} 

//n细分取点

//在队列的每两个点中线性加入x个点
void Point_Queue_Lerp(Point_Queue *q, uint16_t x)
{
    if(q->size < 2 || x == 0) return;
    
    // 临时缓存原数据
    Pixel_Point buffer[TARGET_QUEUE_SIZE];
    int orig_size = q->size;
    for(int i=0; i<orig_size; i++){
        buffer[i] = Point_Queue_Dequeue(q);
    }

    // 重构队列
    for(int i=0; i<orig_size-1; i++){
        Pixel_Point p1 = buffer[i];
        Pixel_Point p2 = buffer[i+1];
        
        // 插入p1和中间点（不含p2）
        Point_Queue_Enqueue(q, p1);
        for(int j=1; j<=x; j++){ 
            float ratio = (float)j/(x+1);
            Point_Queue_Enqueue(q, Lerp_Pixel(p1, p2, ratio));
        }
    }
    
    // 补入最后一个点
    Point_Queue_Enqueue(q, buffer[orig_size-1]);
}


