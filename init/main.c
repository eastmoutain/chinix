#include <console.h>
#include <stdio.h>

char buf[1024] = "123456";

int main()
{
    console_init();
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\n");
    printf("kdflsjdfklsdkfsldk\n");
    
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\r\n");
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\r\n");
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\r\n");
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\r\n");
    printf("kdflsjdfklsdkfsldk\r\n");
    printf("kdflsjdfklsdkfsldk\r\n");
    printf("hhhhhhhhhhhhhhhhhheeeeeeeeeeeeeeeeeeeeee\r\n");
    printf("kdflsjdfklsdkfsldk\r\n");
    printf("kdflsjdfklsdkfsldk\r\n");
    printf("kdflsjdfklsdkfsldk\r\n");
    *buf = 1;
    return 0;
}
