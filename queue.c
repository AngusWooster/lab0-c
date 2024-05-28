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
    struct list_head *h = test_malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(h);
    return h;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    element_t *elem, *safe;
    list_for_each_entry_safe (elem, safe, head, list) {
        free(elem->value);
        list_del(&elem->list);
        free(elem);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (s == NULL) {
        return false;
    }
    element_t *elem = test_malloc(sizeof(element_t));
    if (elem == NULL) {
        return false;
    }
    elem->value = test_malloc(sizeof(char) * (strlen(s) + 1));
    if (elem->value == NULL) {
        free(elem);
        return false;
    }
    strncpy(elem->value, s, strlen(s) + 1);
    list_add(&elem->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (s == NULL) {
        return false;
    }
    element_t *elem = test_malloc(sizeof(element_t));
    if (elem == NULL) {
        return false;
    }
    elem->value = test_malloc(sizeof(char) * (strlen(s) + 1));
    if (elem->value == NULL) {
        free(elem);
        return false;
    }
    strncpy(elem->value, s, strlen(s) + 1);
    list_add_tail(&elem->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }

    element_t *elem = list_first_entry(head, element_t, list);
    list_del_init(&elem->list);
    strncpy(sp, elem->value, bufsize);
    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }

    element_t *elem = list_last_entry(head, element_t, list);
    list_del_init(&elem->list);
    strncpy(sp, elem->value, bufsize);
    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL)
        return 0;

    int len = 0;
    struct list_head *node;
    list_for_each (node, head) {
        len++;
    }
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (list_empty(head)) {
        return false;
    }

    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    element_t *elem = list_entry(slow, element_t, list);
    free(elem->value);
    free(elem);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *list_middle(struct list_head *head)
{
    if (list_empty(head)) {
        return NULL;
    }
    struct list_head *fast = head->next, *slow = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (list_empty(head) || list_is_singular(head)) {
        return;
    }
    /* compute the pivot element */
    element_t *pivot_elem = NULL;
    {
        element_t *first_elem = list_first_entry(head, element_t, list);
        element_t *mid_elem = list_entry(list_middle(head), element_t, list);
        element_t *last_elem = list_last_entry(head, element_t, list);
        if (strcmp(last_elem->value, first_elem->value) >= 0) {
            pivot_elem =
                strcmp(first_elem->value, mid_elem->value) >= 0  ? first_elem
                : strcmp(mid_elem->value, last_elem->value) >= 0 ? last_elem
                                                                 : mid_elem;
        } else {
            pivot_elem =
                strcmp(last_elem->value, mid_elem->value) >= 0    ? last_elem
                : strcmp(mid_elem->value, first_elem->value) >= 0 ? first_elem
                                                                  : mid_elem;
        }
    }
    LIST_HEAD(left);
    LIST_HEAD(right);
    list_del(&pivot_elem->list);

    element_t *elem, *safe;
    list_for_each_entry_safe (elem, safe, head, list) {
        list_del(&elem->list);
        int rc = strcmp(elem->value, pivot_elem->value);
        list_add_tail(&elem->list, ((rc >= 0) ? &right : &left));
    }
    q_sort(&left, descend);
    q_sort(&right, descend);
    /* merge */
    if (descend) {
        list_splice_tail(&right, head);
        list_add_tail(&pivot_elem->list, head);
        list_splice_tail(&left, head);
    } else {
        list_splice_tail(&left, head);
        list_add_tail(&pivot_elem->list, head);
        list_splice_tail(&right, head);
    }
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
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
