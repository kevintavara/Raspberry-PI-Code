#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(void)
{
    FILE * fp;
    char * line = NULL;
    char time[5];
    char x[9];
    char y[9];
    char z[9];
    float xM;
    float yM;
    float zM;
    char i = 5;
    char j = 0;
    bool flag1 = false;
    bool flag2 = false;
    size_t len = 0;
    ssize_t read;

    fp = fopen("STK.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu:\n", read);
        time[0] = line[0];
        time[1] = line[1];
        time[2] = line[2];
        time[3] = line[3];
        time[4] = line[4];
        time[5] = '\0';
        printf("Time: %s\n", time);
        while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (line[i] == ' ') {
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    x[j] = line[i];
                    i++;
                    j++;
                }
            }
        
        }
        flag1 = false;
        flag2 = false;
        
        while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (line[i] == ' ') {
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    y[j] = line[i];
                    i++;
                    j++;
                }
            }
        
        } 
        flag1 = false;
        flag2 = false;
        while(!flag2) {
            if (line[i] == ' ' && !flag1) {
                i++;
            }
            else {
                if (i == 56) {
                    i = 5;
                    j = 0;
                    flag2 = true;
                }    
                else {
                    flag1 = true;
                    z[j] = line[i];
                    i++;
                    j++;
                }
            }
        
        } 
        flag1 = false;
        flag2 = false;
        x[8] = '\0';
        y[7] = '\0';
        xM = atof(x);
        yM = atof(y);
        zM = atof(z);
        printf("XMag: %.2f\n", xM);
        printf("YMag: %.2f\n", yM);
        printf("ZMag: %.2f\n", zM);
    }
    
    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
}








