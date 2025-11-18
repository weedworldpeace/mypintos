/*
  File for 'max-threads' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"

void test_max_mem_malloc(void) 
{
  for (int i = 1;; i++) {
    void* ptr = malloc(256);
    if (ptr == NULL) {
      break;
    } else {
      msg("%d malloc", i);
    }
  }
}

void test_max_mem_calloc(void) 
{
  for (int i = 1;; i++) {
    void* ptr = calloc(sizeof(int), 128);
    if (ptr == NULL) {
      break;
    } else {
      msg("%d calloc", i);
    }
  }
}

void test_max_mem_palloc(void) 
{
  for (int i = 1;; i++) {
    void* ptr = palloc_get_page(PAL_ZERO);
    if (ptr == NULL) {
      break;
    } else {
      msg("%d palloc", i);
    }
  }
}
