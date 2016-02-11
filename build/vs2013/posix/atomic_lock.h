#pragma once

int __sync_fetch_and_sub(int *p, int n);
int __sync_fetch_and_add(int *p, int n);
int __sync_add_and_fetch(int *p, int n);
int __sync_sub_and_fetch(int *p, int n);
int __sync_lock_test_and_set(int *p, int n);
void __sync_lock_release(int *p);
void __sync_synchronize();
char __sync_bool_compare_and_swap(int *p, int value, int compare);
int __sync_and_and_fetch(int *p, int n);


