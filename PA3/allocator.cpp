#include <iostream>
#include <cstring>
#include <stdlib>

#include "Samplerun1"
#include "Samplerun2"


int main() {

int *arr= malloc(sizeof(sample1)*5);
int *arr2= malloc(sizeof(sample2)*5);
sample1=Samplerun1;
sample2=Samplerun2;

sample1.deallocate();
sample2.deallocate();

sample1.allocate();
sample2.allocate();
free(arr);
free(arr2);

  return 0;
}