#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
static void _q_nodes_free(struct list_head *head)
{
    while (!list_empty(head)) {
        struct list_head *node = head->next;
        list_del_init(node);
        element_t *elem = list_entry(node, element_t, list);
        free(elem->value);
        free(elem);
    }
}

void q_free(struct list_head *head)
{
    if (head == NULL) {
        return;
    }
    _q_nodes_free(head);
    free(head);
}

/* Insert an element at head of queue */
static element_t *_elem_create(char *s)
{
    int size = strlen(s) + 1;
    element_t *elem = malloc(sizeof(element_t));
    if (elem == NULL) {
        return NULL;
    }
    INIT_LIST_HEAD(&elem->list);
    elem->value = malloc(sizeof(char) * (strlen(s) + 1));
    if (elem->value == NULL) {
        free(elem);
        return NULL;
    }
    strncpy(elem->value, s, size);
    return elem;
}

bool q_insert_head(struct list_head *head, char *s)
{
    element_t *elem = _elem_create(s);
    if (elem) {
        list_add(&elem->list, head);
        return true;
    }
    return false;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *elem = _elem_create(s);
    if (elem) {
        list_add_tail(&elem->list, head);
        return true;
    }
    return false;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *elem = list_first_entry(head, element_t, list);
    strncpy(sp, elem->value, bufsize);
    sp[bufsize - 1] = '\0';
    list_del_init(head->next);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *elem = list_last_entry(head, element_t, list);
    strncpy(sp, elem->value, bufsize);
    list_del_init(head->prev);
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    struct list_head *node;
    int size = 0;

    list_for_each (node, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/

    if (list_empty(head)) {
        return false;
    }

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        fast = fast->next->next;
        slow = slow->next;
    }

    struct list_head *deleted_node = slow;
    list_del(deleted_node);
    element_t *elem = list_entry(deleted_node, element_t, list);
    q_release_element(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    LIST_HEAD(duplicated_head);
    struct list_head *cur_node = head->next;

    while (cur_node != head && cur_node->next != head) {
        struct list_head *node = cur_node->next;
        element_t *cur_elem = list_entry(cur_node, element_t, list);
        element_t *elem = list_entry(node, element_t, list);

        if (strcmp(cur_elem->value, elem->value) == 0) {
            struct list_head *next = node->next;
            list_move(cur_node, &duplicated_head);
            list_move(node, &duplicated_head);
            node = next;
            while (node != head) {
                elem = list_entry(node, element_t, list);
                if (strcmp(cur_elem->value, elem->value) == 0) {
                    next = node->next;
                    list_move(node, &duplicated_head);
                    node = next;
                } else {
                    break;
                }
            }
            cur_node = node;
        } else {
            cur_node = cur_node->next;
        }
    }
    _q_nodes_free(&duplicated_head);
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    LIST_HEAD(new_head);
    struct list_head *node = head->next;
    while (node != head && node->next != head) {
        struct list_head *prev = node;
        struct list_head *next = node->next;
        node = next->next;
        list_del(prev);
        list_del(next);
        list_add_tail(next, &new_head);
        list_add_tail(prev, &new_head);
    }
    if (node != head) {
        list_del(node);
        list_add_tail(node, &new_head);
    }
    list_splice_tail(&new_head, head);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    LIST_HEAD(new_head);
    struct list_head *node = head->next;
    while (!list_empty(head)) {
        struct list_head *next = node->next;
        list_del_init(node);
        list_add(node, &new_head);
        node = next;
    }
    list_splice_tail(&new_head, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (k == 0) {
        return;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    int size = q_size(head);
    int group = size / k;
    if (group == 0) {
        return;
    }
    LIST_HEAD(new_head);
    struct list_head *node = head->next;
    /* Init heads */
    for (int i = 0; i < group; i++) {
        LIST_HEAD(tmp);
        for (int n = 1; n < k; n++) {
            node = node->next;
        }
        struct list_head *next = node->next;
        list_cut_position(&tmp, head, node);
        q_reverse(&tmp);
        list_splice_tail(&tmp, &new_head);
        node = next;
    }
    list_splice(&new_head, head);
}

/*
split:
    123 45
    12 34
    12 3
    1
*/

static int list_middle_size(int list_size)
{
    return list_size / 2 + (list_size & 1);
}

struct list_head *list_middle(struct list_head *head, int list_size)
{
    int size = list_middle_size(list_size);
    struct list_head *node = head;
    for (int i = 0; i < size; i++) {
        node = node->next;
    }
    return node;
}

static void _q_merge(struct list_head *h1,
                     struct list_head *h2,
                     struct list_head *merged_head,
                     bool descend)
{
    while (!list_empty(h1) && !list_empty(h2)) {
        element_t *left_elem = list_first_entry(h1, element_t, list);
        element_t *right_elem = list_first_entry(h2, element_t, list);

        struct list_head *node = h2->next;
        if ((!descend && strcmp(left_elem->value, right_elem->value) <= 0) ||
            (descend && strcmp(left_elem->value, right_elem->value) > 0)) {
            node = h1->next;
        }
        list_del(node);
        list_add_tail(node, merged_head);
    }
    list_splice_tail_init(!list_empty(h1) ? h1 : h2, merged_head);
}

static void _q_sort(struct list_head *head, bool descend, int list_size)
{
    if (list_size == 0 || list_size == 1) {
        return;
    }
    LIST_HEAD(left_head);
    LIST_HEAD(right_head);
    /* Split List */
    int left_size = list_middle_size(list_size);
    struct list_head *node = list_middle(head, list_size);
    list_cut_position(&left_head, head, node);
    list_splice_init(head, &right_head);

    _q_sort(&left_head, descend, left_size);
    _q_sort(&right_head, descend, list_size - left_size);
    /* merged */
    _q_merge(&left_head, &right_head, head, descend);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (list_empty(head)) {
        return;
    }
    _q_sort(head, descend, q_size(head));
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    LIST_HEAD(removed_head);
    q_reverse(head);

    struct list_head *cur_node = head->next;
    while (!list_empty(head) && cur_node->next != head) {
        struct list_head *node = cur_node->next;

        element_t *cur_elem = list_entry(cur_node, element_t, list);
        element_t *elem = list_entry(node, element_t, list);
        if (strcmp(cur_elem->value, elem->value) >= 0) {
            list_move_tail(node, &removed_head);
        } else {
            cur_node = node;
        }
    }
    _q_nodes_free(&removed_head);
    q_reverse(head);
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    int list_size = q_size(head);

    struct list_head *node = head->next->next;
    queue_contex_t *q1 = list_entry(head->next, queue_contex_t, chain);
    while (list_size > 1) {
        LIST_HEAD(merged_head);

        struct list_head *next = node->next;
        queue_contex_t *q2 = list_entry(node, queue_contex_t, chain);

        _q_merge(q1->q, q2->q, &merged_head, descend);
        list_splice_tail(&merged_head, q1->q);

        node = next;
        list_size--;
    }
    return 0;
}
