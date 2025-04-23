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