#include "oslabs.h"
#include <stdlib.h>

struct RCB findClosestRCB(struct RCB queue[], int queue_cnt, int current_cylinder) {
    int minDistance = abs(queue[0].cylinder - current_cylinder);
    int closestIndex = 0;
    for (int i = 1; i < queue_cnt; i++) {
        int distance = abs(queue[i].cylinder - current_cylinder);
        if (distance < minDistance || (distance == minDistance && queue[i].arrival_timestamp < queue[closestIndex].arrival_timestamp)) {
            minDistance = distance;
            closestIndex = i;
        }
    }
    return queue[closestIndex];
}

// FCFS
struct RCB handle_request_arrival_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    if (current_request.request_id == 0) {
        return new_request;
    } else {
        request_queue[(*queue_cnt)++] = new_request;
        return current_request;
    }
}

struct RCB handle_request_completion_fcfs(struct RCB request_queue[QUEUEMAX], int *queue_cnt) {
    if (*queue_cnt == 0) {
        return (struct RCB){0};
    }
    struct RCB next_request = request_queue[0];
    for (int i = 1; i < *queue_cnt; i++) {
        request_queue[i - 1] = request_queue[i];
    }
    (*queue_cnt)--;
    return next_request;
}

// SSTF
struct RCB handle_request_arrival_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    return handle_request_arrival_fcfs(request_queue, queue_cnt, current_request, new_request, timestamp);
}

struct RCB handle_request_completion_sstf(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder) {
    if (*queue_cnt == 0) {
        return (struct RCB){0};
    }

    struct RCB next_request = findClosestRCB(request_queue, *queue_cnt, current_cylinder);

    for (int i = 0; i < *queue_cnt; i++) {
        if (request_queue[i].request_id == next_request.request_id) {
            for (int j = i; j < *queue_cnt - 1; j++) {
                request_queue[j] = request_queue[j + 1];
            }
            break;
        }
    }
    (*queue_cnt)--;

    return next_request;
}

// LOOK
struct RCB handle_request_arrival_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, struct RCB current_request, struct RCB new_request, int timestamp) {
    return handle_request_arrival_fcfs(request_queue, queue_cnt, current_request, new_request, timestamp);
}

struct RCB handle_request_completion_look(struct RCB request_queue[QUEUEMAX], int *queue_cnt, int current_cylinder, int scan_direction) {
    if (*queue_cnt == 0) {
        return (struct RCB){0}; 
    }

    int closestIndex = -1;
    int minDistance = QUEUEMAX; 

    for (int i = 0; i < *queue_cnt; i++) {
        int distance = abs(request_queue[i].cylinder - current_cylinder);
        if ((scan_direction == 1 && request_queue[i].cylinder >= current_cylinder && distance <= minDistance) ||
            (scan_direction == 0 && request_queue[i].cylinder <= current_cylinder && distance <= minDistance)) {
            minDistance = distance;
            closestIndex = i;
        }
    }
    if (closestIndex == -1) { // Change direction if no requests in the current direction
        scan_direction = 1 - scan_direction;
        return handle_request_completion_look(request_queue, queue_cnt, current_cylinder, scan_direction); 
    }

    struct RCB next_request = request_queue[closestIndex];

    for (int i = closestIndex; i < *queue_cnt - 1; i++) {
        request_queue[i] = request_queue[i + 1];
    }
    (*queue_cnt)--;
    return next_request;
}
