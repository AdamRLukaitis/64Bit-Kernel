#include <stdio.h>
void rec()
{
	printf("1");
	rec();
}



int main(int argc, char* argv[]) 
{

  rec();
  return 0;
}

