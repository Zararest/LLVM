int smax(int *arr, unsigned int size) {
  if (size == 0)
    return 0;
  int max = arr[0];
  for (unsigned int i = 0; i < size; i++) 
    if (arr[i] > max)
      max = arr[i];
  
  return max;
}