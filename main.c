#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printMatrix(int** output, int m, int rLen, int algorithm) {
  // Prints the algorithm used
  if (algorithm == 1) printf("FIFO\n   ");
  if (algorithm == 2) printf("Optimal\n   ");
  if (algorithm == 3) printf("LRU\n   ");

  // Prints the reference string 
  for (int i = 0; i < rLen; i++) {
    printf(" %d ",output[0][i]);
  }
  printf("\n-------------------------------------------------------------------\n");

  // Prints page frames
  for (int i = 1; i <= m; i++) {
    printf("   ");
    for (int j = 0; j < rLen; j++) {
      if (output[i][j] < 0) printf("%d ",output[i][j]);
      else printf(" %d ",output[i][j]);
    }
    printf("\n");
  }

  // Prints page faults
  int faultCount = 0;
  printf("   ");
  for (int i = 0; i < rLen; i++) {
    if (output[m+1][i] == 1) {
      printf(" p ");
      faultCount++;
    }
    else printf("   ");
  }
  printf("\n-------------------------------------------------------------------\n");
  printf("%d page faults.\n\n",faultCount);
}

void FIFO(int** output, int* frames, int m, int rLen) {
  //Running through the reference string and current page frames
  for (int j=0; j<rLen; j++) {
    for (int i=1; i<=m; i++) {
      //If any current page frame contains the virtual page being called, page fault is set to null and broken out of loop
      if (output[0][j] == frames[i]) {
        output[m+1][j] = 0;
        break; 
      }
      //If no current page frame contains the virtual page being called, page fault is triggered
      else output[m+1][j] = 1;
    }
    //If page fault concluded, FIFO method implemented to remove oldest-called page and add current-called virtual page
    if (output[m+1][j]) {
        for (int i=m-1; i>0; i--) frames[i] = frames[i-1];
        frames[0] = output[0][j];
    }
    //Copy new current page frame into corresponding column of the output matrix
    for (int i=1; i<=m; i++) output[i][j] = frames[i-1];
  }
  printMatrix(output,m,rLen,1);
}

void optimal(int** output, int m, int rLen) {
  int fault = 0, i = 0, replace;
  int count[m];
  
  // Filling the initial holes and indicating page faults
  while (i < m) {
    for (int j = i; j < m; j++) {
      output[i+1][j] = output [0][i];
    }
    i++;
  }
  for (int s = 0; s < m; s++) output[m+1][s] = 1;

  // Incrementing through the rest of the array
  for (i = m; i < rLen; i++) { 
    // Check if the page already exists in the frame
    for (int j = 0; j < m; j++) {
      if (output[0][i] == output[j+1][i-1]) {  // breaks if it exists
        output[m+1][i] = 0; 
        break;
      }
      else output[m+1][i] = 1;  // raises page fault if it does not exist
    }
    if (output[m+1][i]) {
      // Goes through the page frame and sets the count of the existing till the next reference
      for (int k = 0; k < m; k++) {
        int p = i+1, q = 1;
        count[k] = 0;
        // increases count until the same page is reached or the full reference string has been searched
        while (output[k+1][i-1] != output[0][p] && p < rLen) {
          count[k] = q;  
          q++; p++;
        }
      }
      // Determines which page to replace by the greatest length
      int max = count[0]; replace = 0;
      for (int k = 1; k < m; k++) {
        if (count[k] > max) {
          replace = k;
          max = count[k];
        }
      }
      // Replaces the page with current request and sets others to the old pages
      for (int k = 0; k < m; k++) {
        if (k == replace) output[k+1][i] = output[0][i];
        else output[k+1][i] = output[k+1][i-1];
      }
    }
    else {  
      // Sets page frame to the same as the one before if no page fault occured
      for (int k = 0; k < m; k++) output[k+1][i] = output[k+1][i-1];
    }
  }
  printMatrix(output,m,rLen,2);
}

void LRU(int** output, int* frames, int m, int rLen) {
  int fault=0, i=0, replace, count[m],tempOldest;
  int indexList[m];
  //set all elements in the array to 0
  for (int j=0;j<m;j++){
    indexList[j]=0;
  }
  // Filling the initial holes and indicating page faults
  while (i < m) {
    for (int j = i; j < m; j++) {
      output[i+1][j] = output [0][i];
      indexList[j] = i;
    }
    i++;
  }
  for (int s = 0; s < m; s++) output[m+1][s] = 1;
  // Incrementing through the rest of the array
  for (int i=m; i<rLen; i++){
    // Check if the page already exists in the frame
    for (int j = 0; j < m; j++) {
      if (output[0][i] == output[j+1][i-1]) {  // breaks if it exists
        output[m+1][i] = 0;
        indexList[j] = i;
        break;
      }
      else output[m+1][i] = 1;  // raises page fault if it does not exist
    }
    //if there is a page fault 
    if(output[m+1][i]){
      //find the oldest index 
      tempOldest=indexList[0];
      replace = 0;
      for (int x=0;x<m;x++){
        if (tempOldest>indexList[x]){
          tempOldest=indexList[x];
          replace = x;
        }
      }
      //replace the oldest with the newest entry and copy the ones that remain unchanged
      for (int k=0;k<m;k++){
        if (k==replace){
          output[k+1][i] = output[0][i];
          indexList[k]=i;
        }
        else{
          output[k+1][i] = output[k+1][i-1];
        }
      }
    }
    else {  
      // Sets page frame to the same as the one before if no page fault occured
      for (int k = 0; k < m; k++) 
        {
          output[k+1][i] = output[k+1][i-1];
        }
    }
    
  }
  printMatrix(output,m,rLen,3);
}

int main(void) {
  int n, m, rLen = 0, rString[20];

  // Assigning file values to variables
  FILE *file = fopen ("sampledata.txt", "r");
  char *line = NULL, *token;
  int i = 0;
  size_t len = 0;
  while ((getline(&line, &len, file)) != -1){
    // First line containing n and m
    if (i == 0) {
      int j = 0;
      token = strtok(line," ");  // splits string at spaces
      while (token != NULL) {
        if (j == 0) {n = atoi(token);}
        else if (j == 1) {m = atoi(token);}
        token = strtok(NULL," "); j++;
      }
    }
    // Second line containing the reference string
    if (i == 1) {
      token = strtok(line," ");  // splits string at spaces
      while (token != NULL) { 
        if (atoi(token) == -1) break;  // -1 indicates the end
        rString[rLen] = atoi(token);
        token = strtok(NULL," "); rLen++;
      }
    }
    i++;
  }
  fclose(file);

  // dynamically allocate initial page frame matrix of holes (-1)
  int *frames = (int*)malloc(n*sizeof(int));
  for (int i = 0; i < n; i++) frames[i] = -1;

  // dynamically allocate matrix to be printed
  int** output = (int**)malloc((m+2)*sizeof(int*)); 
  for (int i = 0; i < m+2; i++) {
    output[i] = (int*)malloc(rLen * sizeof(int));
  }
  for (int i=0; i<rLen; i++) output[0][i] = rString[i];  // filling first line with the reference string
  for (int i = 0; i < m-1; i++) {
    for (int j = 2; j < m+1; j++) output[j][i] = -1;
  }

  // initial print statements
  printf("Number of pages: %d\n",n);
  printf("Number of frames: %d\n",m);
  printf("Size of the reference string: %d\n",rLen);
  printf("Reference String:\n\t");
  for (int i = 0; i < rLen; i++) {printf("%d ",output[0][i]);}
  printf("\n\n");
  
  FIFO(output, frames, m, rLen);
  optimal(output, m, rLen); 
  LRU(output, frames, m, rLen); 
  
  return 0;
}