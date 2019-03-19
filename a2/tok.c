#include <stdio.h> 
#include <string.h> 
  
int main() 
{ 
    char str[] = "cat > lall.c < las"; 
    char* token; 
    char* rest = str; 
    char *rest1;
    subtoken = strtok_r(token, ">< ", &token);
    // printf("%s %s %s \n", rest,rest1, token);
  
    while (token!=NULL) 
    {
    	token = strtok_r(NULL, ">< ", &rest);
    	// printf("%s %s %s\n", rest,rest1, token);
 		printf("%lu\n", rest-token + strlen(token) );
    }
  
    return (0); 


    // char str[] = "Geeks-for-all"; 
  
    // // Returns first token 
    // char* token = strtok(str, "-"); 
  
    // // Keep printing tokens while one of the 
    // // delimiters present in str[]. 
    // while (token != NULL) { 
    // 	printf("%s %s %lu\n", token, str, strlen(token));
    //     // printf("%s\n", str); 
    //     token = strtok(NULL, "-"); 
    // } 
  
    return 0; 
} 