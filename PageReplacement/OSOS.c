#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
/*
출력포맷

처리기법 이름 
총 page fault 횟수
메모리 상태 변화 과정 
*/
int Queue[1000];
int front = 0;
int rear = 0;
void insQue(int data)
{
	Queue[rear++]=data;
}
int delQue()
{
	return Queue[front++];
}
int stack[1000];
int top = -1;
int bottom = 0;
int push(int data)
{
	int i; 
	int c = 0;
	for (i=bottom; i<top+1;i++)
	{
		if(stack[i]==data)
		{
			c= 1;
			break;
		}
	}
	if(c)
	{
		for(i ; i<top;i++)
		{
			int temp = stack[i];
			stack[i]=stack[i+1];
			stack[i+1]=temp;
		}
	}
	else
		stack[++top]=data;
}
int pop()
{
	return stack[bottom++];
}
int searchLFU(int* freq,int M)
{
	int min = 1000000;
	int i;
	for(i=0;i<M;i++)
	{
		if(freq[i]<min)
			min=freq[i];
		
	}
	for(i=0;i<M;i++)
	{
		if(freq[i]==min)
		{
			//freq[i]=1;
			return i;
		}
	}
}
void printFrame(int** , int , int );
int searchMIN(int** , int , int , int , int* , int );
void pageMIN(int , int ,int , int* );
void pageFIFO(int, int, int , int* );
void pageLRU(int, int, int , int* );
void pageLFU(int, int, int , int* );
void pageWS(int, int, int, int*);
int main()
{
	FILE* fp1 = fopen("input.txt","rt");
	int N,M,W,K;
	int i,j;
	int* pages;
	fscanf(fp1,"%d %d %d %d",&N,&M,&W,&K);
	pages = (int*)malloc(sizeof(int)*K);
	for(i=0; i<K;i++)
	{
		fscanf(fp1,"%d",&pages[i]);
	
	}
	if(K<1||N<1||M<1)
	{
		printf("입력 정보가 충분하지 않습니다. \n");
		return -1;
	}
	printf("\nMIN===\n");
	for(i=0; i<K;i++)
		printf("%d ",pages[i]);
	printf("\n");
	pageMIN(M,K,W,pages);
	printf("\nFIFO==\n");
	for(i=0; i<K;i++)
		printf("%d ",pages[i]);
	printf("\n");
	pageFIFO(M,K,W,pages);
	printf("\nLRU===\n");
	for(i=0; i<K;i++)
		printf("%d ",pages[i]);
	printf("\n");
	pageLRU(M,K,W,pages);
	printf("\nLFU===\n");
	for(i=0; i<K;i++)
		printf("%d ",pages[i]);
	printf("\n");
	pageLFU(M,K,W,pages);
	printf("\nWS===\n");
	for(i=0; i<W;i++)
	{
		printf("0 ");
	}
	for(i=0; i<K;i++)
		printf("%d ",pages[i]);
	printf("\n");
	pageWS(N,K,W,pages);
	
}
void printFrame(int** arr, int time, int M)
{
	int i,j;
	for(i=0; i<M; i++)
	{
		for(j=0; j<time;j++)
		{
			if(arr[i][j]!=-1)
			{
				if(arr[i][j]/10000==1)
				{	
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),4);
					printf("%d ",arr[i][j]%10000);
					
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),15);
					
			
				}
				else
					printf("%d ",arr[i][j]%10000);
			}
			else
				printf("E ");
		}
		printf("\n");
	}
}
int searchMIN(int** frame, int M, int time, int index, int* pages, int K)
{
	int i,j; 
	int* arr = (int*)malloc(sizeof(int)*M);
	int count=0;
	int min=100;
	for(i=0;i<M;i++)
	{
		int temp =frame[i][time-1];
		for(j=index ; j<K;j++)
		{
			if(temp==pages[j])
			{
				count++;
			}
			
		}
		arr[i]=count;
		count=0;
	}
	for(i=0;i<M;i++)
	{
		if(min>arr[i])
			min=arr[i];
	}
	for(i=0;i<M;i++)
	{
		if(min==arr[i])
			return i;
	}
}
void pageMIN(int M, int K,int W, int* pages)
{
	int** frame = (int**)malloc(sizeof(int*)*M);//사용할 프레임 크기만큼의 어레이 제작.
	int i, j,k;
	int time = 0; //시간이자, index 
	int pagefault = 1; 
	for(i=0;i<M;i++)
	{
		frame[i] = (int*)malloc(sizeof(int)*100);	
	}
	
	for(i=0;i<M;i++)
	{
		for(j=0; j<100;j++)
		{
			frame[i][j]= -1;//비어있다는 표시 
		}
	}

	frame[0][time++]=pages[0]+10000; 
	
	//실제 MIN 작용
	for(i=1; i<K;i++)
	{
		int refc=0; 
		int emptc=0;
		int temp = pages[i];
		 
		//페이지 확인 후 이미 있는 프레임일 경우 그냥 레퍼런스 
		for(j=0;j<M;j++)
		{
			if(frame[j][time-1]%10000==temp)
			{
				for(k=0;k<M;k++)
				{
				
					frame[k][time]=frame[k][time-1]%10000;
					
				}
				time++;
				refc=1;
				break;
			}
		}
		if(refc)
			continue;
		for(j=0; j<M;j++)//빈자리 있으면 넣음. 
		{
			if(frame[j][time-1]==-1)
			{
				for(k=0;k<M;k++)
				{
					if(k!=j)
						frame[k][time]=frame[k][time-1]%10000;
				}
				frame[j][time++]=temp+10000;
				emptc=1;
				pagefault++;
				break;
			}
			
		}
		if(emptc)
		{	
			continue;
		}
		
		//대체할 프레임을 찾기 
		int curindex = searchMIN(frame, M, time, i, pages, K);
		for(j=0;j<M;j++)
		{
			if(j==curindex)
			{
				frame[j][time]=temp+10000;
			}
			else
				frame[j][time]=frame[j][time-1]%10000;	
			
		}
		pagefault++;
		time++;
	} 
	printf("PageFault = %d\n",pagefault);
	printFrame(frame, time, M);
		
}
void pageFIFO(int M, int K,int W, int* pages)
{
	int** frame = (int**)malloc(sizeof(int*)*M);//사용할 프레임 크기만큼의 어레이 제작.
	int i, j,k;
	int time = 0; //시간이자, index 

	int pagefault = 1; 
	for(i=0;i<M;i++)
	{
		frame[i] = (int*)malloc(sizeof(int)*100);	
	}
	
	for(i=0;i<M;i++)
	{
		for(j=0; j<100;j++)
		{
			frame[i][j]= -1;//비어있다는 표시 
		}
	}

	frame[0][time++]=pages[0]+10000; 
	insQue(0);
	
	//실제 MIN 작용
	for(i=1; i<K;i++)
	{
		int refc=0; 
		int emptc=0;
		int temp = pages[i];
		 
		//페이지 확인 후 이미 있는 프레임일 경우 그냥 레퍼런스 
		for(j=0;j<M;j++)
		{
			if(frame[j][time-1]%10000==temp)
			{
				for(k=0;k<M;k++)
				{
				
					frame[k][time]=frame[k][time-1]%10000;
					
				}
				time++;
				refc=1;
				break;
			}
		}
		if(refc)
			continue;
		for(j=0; j<M;j++)//빈자리 있으면 넣음. 
		{
			if(frame[j][time-1]==-1)
			{
				for(k=0;k<M;k++)
				{
					if(k!=j)
						frame[k][time]=frame[k][time-1]%10000;
				}
				insQue(j);
				frame[j][time++]=temp+10000;
				emptc=1;
				pagefault++;
				break;
			}
			
		}
		if(emptc)
		{	
			continue;
		}
		
		//대체할 프레임을 찾기 
		int curindex = delQue();
		insQue(curindex);
		for(j=0;j<M;j++)
		{
			if(j==curindex)
			{
				frame[j][time]=temp+10000;
			}
			else
				frame[j][time]=frame[j][time-1]%10000;	
			
		}
		pagefault++;
		time++;
	} 
	printf("PageFault = %d\n",pagefault);
	printFrame(frame, time, M);
		
}
void pageLRU(int M, int K,int W, int* pages)
{
	int** frame = (int**)malloc(sizeof(int*)*M);//사용할 프레임 크기만큼의 어레이 제작.
	int i, j,k;
	int time = 0; //시간이자, index 

	int pagefault = 1; 
	for(i=0;i<M;i++)
	{
		frame[i] = (int*)malloc(sizeof(int)*100);	
	}
	
	for(i=0;i<M;i++)
	{
		for(j=0; j<100;j++)
		{
			frame[i][j]= -1;//비어있다는 표시 
		}
	}

	frame[0][time++]=pages[0]+10000; 
	push(0);
	
	//실제 MIN 작용
	for(i=1; i<K;i++)
	{
		int refc=0; 
		int emptc=0;
		int temp = pages[i];
		 
		//페이지 확인 후 이미 있는 프레임일 경우 그냥 레퍼런스 
		for(j=0;j<M;j++)
		{
			if(frame[j][time-1]%10000==temp)
			{
				push(j);
				for(k=0;k<M;k++)
				{
				
					frame[k][time]=frame[k][time-1]%10000;
					
				}
				time++;
				refc=1;
				break;
			}
		}
		if(refc)
			continue;
		for(j=0; j<M;j++)//빈자리 있으면 넣음. 
		{
			if(frame[j][time-1]==-1)
			{
				for(k=0;k<M;k++)
				{
					if(k!=j)
						frame[k][time]=frame[k][time-1]%10000;
				}
				push(j);
				frame[j][time++]=temp+10000;
				emptc=1;
				pagefault++;
				break;
			}
			
		}
		if(emptc)
		{	
			continue;
		}
		
		//대체할 프레임을 찾기 
		int curindex = pop();
		push(curindex);
		for(j=0;j<M;j++)
		{
			if(j==curindex)
			{
				frame[j][time]=temp+10000;
			}
			else
				frame[j][time]=frame[j][time-1]%10000;	
			
		}
		pagefault++;
		time++;
	} 
	printf("PageFault = %d\n",pagefault);
	printFrame(frame, time, M);
		
}
void pageLFU(int M, int K,int W, int* pages)
{
	int** frame = (int**)malloc(sizeof(int*)*M);//사용할 프레임 크기만큼의 어레이 제작.
	int* freq = (int*)malloc(sizeof(int)*M);// 빈도수 저장용 
	int i, j,k;
	int time = 0; //시간이자, index 
	
	int pagefault = 1; 
	for(i=0;i<M;i++)
	{
		freq[i]=0;		
		frame[i] = (int*)malloc(sizeof(int)*100);	
	}
	
	for(i=0;i<M;i++)
	{
		for(j=0; j<100;j++)
		{
			frame[i][j]= -1;//비어있다는 표시 
		}
	}

	frame[0][time++]=pages[0]+10000; 
	freq[0]++;
	//실제 MIN 작용
	for(i=1; i<K;i++)
	{
		int refc=0; 
		int emptc=0;
		int temp = pages[i];
		 
		//페이지 확인 후 이미 있는 프레임일 경우 그냥 레퍼런스 
		for(j=0;j<M;j++)
		{
			if(frame[j][time-1]%10000==temp)
			{
				freq[j]++;
				for(k=0;k<M;k++)
				{
					frame[k][time]=frame[k][time-1]%10000;
					
				}
				time++;
				refc=1;
				break;
			}
		}
		if(refc)
			continue;
		for(j=0; j<M;j++)//빈자리 있으면 넣음. 
		{
			if(frame[j][time-1]==-1)
			{
				freq[j]++;
				for(k=0;k<M;k++)
				{
					if(k!=j)
						frame[k][time]=frame[k][time-1]%10000;
				}
				frame[j][time++]=temp+10000;
				emptc=1;
				pagefault++;
				break;
			}
			
		}
		if(emptc)
		{	
			continue;
		}
		
		//대체할 프레임을 찾기 
		int curindex = searchLFU(freq,M);
		
		for(j=0;j<M;j++)
		{
			if(j==curindex)
			{
				freq[j]++;
				frame[j][time]=temp+10000;
			}
			else
				frame[j][time]=frame[j][time-1]%10000;	
			
		}
		pagefault++;
		time++;
	} 
	printf("PageFault = %d\n",pagefault);
	printFrame(frame, time, M);
		
}
void pageWS(int N, int K, int W, int* pages)
{
	int** frame = (int**)malloc(sizeof(int*)*N);//사용할 페이지만큼 
	int i, j,k;
	int time = 0; //시간이자, index 
	int* newpages;
	newpages = (int*)malloc(sizeof(int)*(K+W));
	int pagefault = 0; 
	
	for(i=0;i<N;i++)
	{
		frame[i] = (int*)malloc(sizeof(int)*100);	
	}
	j=0;
	for(i=0;i<K+W;i++)
	{
		if(i<W)
			newpages[i]=0;
		else
		{
			if(pages[j]<N)
			{
				newpages[i]=pages[j++];
		
			}
			else{
				printf("페이지 크기가 충분하지 않습니다.\n");
				return; 
			}
		}
			
	}
	for(i=0;i<N;i++)
	{
		for(j=0; j<100;j++)
		{
			frame[i][j]= -1;//비어있다는 표시 
		}
	}
	//0만 W개만큼 들어왔다고 가정
	for (time;time<W;time++)
	{
		frame[0][time]=0;
		
	}
	
	 //실제 MIN 작용
	for(i=W; i<K+W;i++)
	{
	
		int temp = newpages[i];
		//WS 적용
		for(k=time-W;k<time;k++)
		{
			frame[newpages[k]][time]=newpages[k];
		}
		
		//페이지 확인 후 이미 있는 프레임일 경우 그냥 레퍼런스 
		if(frame[temp][time]==-1)
		{
			frame[temp][time]=temp+10000;
			pagefault++;
		}
		time++;
		
		
	} 
	printf("PageFault = %d\n",pagefault);
	printFrame(frame, K+W, N);
	
}
