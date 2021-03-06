#include "List.H"

List *create_iter(int n) {
  List *head=0;
  List *newElement;

  while (n>=0) {
    newElement = new List(n);
    newElement->n = head;
    head = newElement;
    n--;
  }

  return head;
}


// appends second list to tail of first
// PavluSchordanKrall10: new testcase
List* app_iter(List* head, List* q) {
  List* cur = head;

  if (head == NULL) {
    return q;
  }

  while (cur->n != NULL) {
    cur = cur->n;
  }

  cur->n = q;

  return head;
}

int main(int argc, char **argv) {
  List *a = create_iter(6);
  List *b = create_iter(6);

  List *head = app_iter(a, b);

  a->n->n = NULL;
  b->n->n = NULL;
  head->n->n = NULL;

  return 1;
}

