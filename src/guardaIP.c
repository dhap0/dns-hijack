#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "guardaIP.h"
#define calcIndex(a) (calcRange(IP_INI,a))


//void inicia_guardaIP(){
//    guardaIp =(char**)malloc(3200);
//}


//Tested OK
int calcRange(char* ip_ini, char* ip_fin){
    int ip_iniv[4];
    int ip_finv[4];
    int range;
    char delim[]=".";
    char cpy_ini[16];  //Aquesta merda es necessaria pel strtok
    char cpy_final[16];
    strncpy(cpy_ini,ip_ini,16);
    strncpy(cpy_final,ip_fin,16);
    char* ptr_ini = strtok(cpy_ini,delim);
    int i;
    for(i=0; i < 4; i++){
	ip_iniv[i] = atoi(ptr_ini);
	ptr_ini = strtok(NULL,delim);
    }
    char* ptr_fin = strtok(cpy_final,delim);
    for(i=0; i < 4; i++){
	ip_finv[i] = atoi(ptr_fin);
	ptr_fin = strtok(NULL,delim);
    }
    range = (ip_finv[0]-ip_iniv[0])*256*256*256
	+ (ip_finv[1]-ip_iniv[1])*256*256
	+ (ip_finv[2]-ip_iniv[2])*256
	+ (ip_finv[3]-ip_iniv[3]);
    return range;
}

int r_findValue(char* s){
   int index = calcIndex(s);
   return registry[index]; //em retornarà 1 si l'IP està registrada.
}

int r_clear (){
    memset(registry, 0, r_length);
    return 1;
}

int r_add (char* ip){
    int ind  = calcIndex(ip);
    registry[ind] = 1;
    return ind;
}

int r_get_length(){
    return r_length;
}

int init_guardaIP(char* ip_ini, char* ip_fin){ 
    r_length = calcRange(ip_ini,ip_fin);
    registry = malloc(r_length*sizeof(int));
    return 1;
}

////tested OK
//void parseValues(char* valors, char result[][16]){
//    int i;
//    int c = 0;
//    int ip = 0;
//    for(i=0; i < strlen(valors); i++){	    //canvio /n per nullbytes
//	if (valors[i] == '\n'){
//	    ip++;
//	    c = 0;
//	} else {
//	    char o = valors[i];
//	    result[ip][c] = o;
//	    c++;
//	}
//    }
//}

//int changeValues(char* values){
//    int i; 
//    free((void*)guardaIp);
//    guardaIp = (char**)malloc(3200);
//    guardaIp = parseValues(values);
//    return 1;
//}