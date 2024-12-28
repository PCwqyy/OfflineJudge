#include<cstdio>
#include<cstdlib>
#include<sys/time.h>
int timestamp()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (tv.tv_sec*1000 + tv.tv_usec/1000);
}
int main()
{
	srand(timestamp());
	printf("%d %d",rand()*rand(),rand()*rand());
	return 0;
}
