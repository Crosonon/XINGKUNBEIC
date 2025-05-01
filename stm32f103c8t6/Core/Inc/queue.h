#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "Coordinate.h"

#define TARGET_QUEUE_SIZE 30  // 队列容量


// 初始化队列
void Point_Queue_Init(Point_Queue *q);

// 检查队列是否为空
bool Point_Queue_IsEmpty(Point_Queue *q);

// 检查队列是否已满
bool Point_Queue_IsFull(Point_Queue *q);

// 入队（添加目标点）
void Point_Queue_Enqueue(Point_Queue *q, Pixel_Point point);

// 出队（移除并返回队首目标点）
Pixel_Point Point_Queue_Dequeue(Point_Queue *q);

// 查看队首元素（不移除）
Pixel_Point Point_Queue_Peek(Point_Queue *q);
