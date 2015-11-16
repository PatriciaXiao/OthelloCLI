
/*
 * othelloai.cpp
 *
 *  Created on: 2012-12-22
 *      Author: patriciaxiao
 */

// 黑白棋的字符界面版
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>

#include <iostream>
using namespace std;

// 空位置设为0 ，黑子下的位置设为1 ，白子下的位置设为2
#define EMPTY		'0'
#define BLACK		'1'
#define WHITE	    '2'
#define SCALE		 8						//黑白棋规模一般是 8 X 8
#define DEPTH        6                      //搜索深度
#define FileNameLEN  60

#define inf          0xfffffff      //最后似乎没派上什么用场？

char HUMAN;
char COMPUTER;

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];  //读盘之前放进来处理用的

void DrawBoard();

void SaveBoard()		// 把棋盘保存到文件中
{
	system("CLS");
	DrawBoard();
	int i, j, k = 0;
	FILE *fp = NULL;    //初始谁也不指向
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE], ch;
	char order[100];
	int l_order;

	while(fp==NULL)
	{
		printf("Please input the name of the file in which the board to be saved\n");
		printf("( No more than 60 letters )");
		printf("请输入文件名 ( 小于60字符 ) \n");
		gets(filename);
		
		while (access(filename, 0) == 0)   //已经存在
		{
			printf("The file named '%s' has already existed, would you like to rewrite the file?/n", filename);
			printf( "文件'%s'已经存在了，需要覆盖它吗？（Y/N/Q）\n\n", filename);
			printf( "\tY=yes(是的，覆盖)\nN=no(不覆盖，更改名称)\nQ=quit saving(放弃存盘)\n\n");
			printf("Your choice:\n");
			printf("请给出您的选择：\n\n");
			while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //截取串中第一个不为空格的字符
			    {
				    if(order[i]==' ')
				    {
					    ch=order[i+1];
				    }
				    else
					    break;
			    }
				if((ch=='N')||(ch=='n')||(ch=='y')||(ch=='Y')||(ch == 'q')||(ch == 'Q'))
				{
					break;
				}
				else
				{
					printf("\nPlease input an order(Y/N/Q)(nomatter capital or not) that could be understood.\n");
					printf("请按照规定的形式输入，不拘大小写：\n\n");
				}
			}
			if (ch == 'Y' || ch == 'y')
				break;     //那就用这个名字了（管他呢……人家都说可以了）
			else
				if(ch == 'q' || ch == 'Q' )
					return;
			    else 
				{
					printf("\nYour new choice of the file's nams:\n");
					printf("请重新输入文件名：\n\n");
					gets(filename);
					continue;   //继续问他要用什么名字……
				}
		}

		err  = fopen_s( &fp, filename, "w+" );

		if( err == 0 )
		{
			printf("File '%s' has been opened successfully, waiting to save the board into it\n",filename);
			printf( "文件'%s'成功打开，等待写入\n", filename );
			system("PAUSE");
		}
		else 
		{
			printf("The effort we paid trying to open the file '%s' has failed, what returned is an error code %d",filename,err);
			printf( "文件'%s'打开失败，返回错误码 %d \n\n", filename, err);
			printf("Would you like to input another file's name in order to save the board(Y/N)?");
			printf("\n您是否选择重新输入正确的文件名以存盘？（Y/N）\n\n");
			printf("Y: Retry saving and input a file name again\n  重新输入文件名\n\n");
			printf("N: Give up saving the board\n  放弃存盘\n\n");
            while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //截取串中第一个不为空格的字符
			    {
				    if(order[i]==' ')
				    {
					    ch=order[i+1];
				    }
				    else
					    break;
			    }
				if((ch=='N')||(ch=='n')||(ch=='y')||(ch=='Y'))
				{
					break;
				}
				else
				{
					printf("\nPlease input an order(Y/N)(no matter capital or not) that could be understood, thank you.\n");
					printf("\n请按要求输入选择，谢谢合作(y/n,大小写不拘)");
				}
			}
		}
	} 

	if (fp == NULL)  //说明是放弃存盘退出的
		return ;

	for(i=0;i<SCALE;i++)      //存盘
	{
		for(j=0;j<SCALE;j++) 
		{
			str[k++] = board[i][j];
		}
		str[k++]='\n';			// 加回车
	}

    fwrite(str,1,(SCALE+1)*SCALE,fp);	// 函数原型size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
							// 注意，这里 str并没有保存"\0"字符串结束符，不能用    fwrite(str,1,strlen(str),fp);	
    fclose(fp);  //记得关上……

	return;
}

//判断是否合法棋盘的系列函数

//递归专用
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;    //出界返回
	if(!b[i][j])  //空值返回
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);   //向八方蔓延
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
	return;
}

//判断棋盘
int board_check(char a[SCALE][SCALE])
{
	int temp[SCALE][SCALE];
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if((a[i][j]==BLACK)||(a[i][j]==WHITE))
			{
				temp[i][j]=1;
			}
			else
				if(a[i][j]==EMPTY)
			    {
				    temp[i][j]=0;
			    }
				else                 //存在非法字符
				{
					return -1;
				}
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
		{
			if(!temp[i][j])  //如果中心没有棋子，一定非法
				return 0;
		}
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(temp[i][j])
				return 0;   //未连成一片，一定不行
		}
	}
	return 1;
}

void ReadBoard()		// 从文件中读入棋盘数据
{
	int i, j, k = 0;
	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE], ch;  //呵呵……这么长的程序都编过来了，最后一个错是设错了数组长度以至于栈溢出……
	char order[100];
	int l_order;
	int okay;
 read:
	ch='0';
	while(fp == NULL) 
	{
		printf("Please input a file's name from which you'd like to get the board\n");
		printf("(warning: no more than 60 letters)\n");
		printf("请输入文件名 ( 小于 60 字符 ) \n\n");
		gets(filename);
		err  = fopen_s( &fp, filename, "r" );// fp = fopen(filename,"r+") ;
		if( err == 0 )
		{
			printf("File '%s' has been opened successfully\n", filename);
			printf( "文件'%s'成功打开\n\n", filename );
			break;
		}
		else 
		{
			if (err == ENOENT)
			{
				printf("Sorry, the file '%s' does not exist\n",filename);
			    printf( "文件'%s'不存在\n\n", filename);
			}
			else
			{
				printf("Returned an error code %d, the file you chose could not open.\n", filename, err);
			    printf( "文件'%s'打开失败，返回错误码 %d \n\n", filename, err);
			}
			printf("Reinput a file's name?(Y/N)\n");
			printf("重新输入文件名？(Y/N)\n\n");
			printf("Y=yes 是\nN=no 否\n\n");
			while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //截取串中第一个不为空格的字符
			    {
				    if(order[i]==' ')
				    {
					    ch=order[i+1];
				    }
				    else
					    break;
			    }
				if((ch=='N')||(ch=='n')||(ch=='y')||(ch=='Y'))
				{
					break;
				}
				else
				{
					printf("\nPlease input an order(Y/N)(no matter capital or not) that could be understood.\n");
					printf("\n请按要求输入选择，谢谢合作(y/n,大小写不拘)");
				}
			}
			if (ch == 'N' || ch == 'n')
				return ;
		}
	}          //文件名可以对应到某文档

	fread(str,1,SCALE*(SCALE+1),fp);	
	// 函数原型size_t fread(void *buffer, size_t size, size_t count, FILE *stream);　

	for (i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++) 
		{
			save_board[i][j] = str[k++];
		}
		k++;					// 去掉回车
	}

    fclose(fp);  //用过就关好习惯

	okay = board_check(save_board);

	if(okay == 1)
	{
		for(i=0;i<SCALE;i++)
		{
			for(j=0;j<SCALE;j++)
			{
				board[i][j] = save_board[i][j];
			}
		}

		return;
	}

	else
	{
		if(okay==0)
		{
			printf("\nI'm sorry but this is an illegal board, please use another one, thank you.");
			printf("\n对不起，您所指定的棋盘明显是错误的棋盘，请重新选择，谢谢合作\n\n");
		}
		else
		{
			printf("\nI'm sorry but there are some characters illegal in the file\n");
			printf("对不起，您所指定的文件中存在本游戏无法识别的符号，无法正常打开\n\n");
		}

	    printf("\nDo you want to give up reading the board (0) , \n\tor select another file to open (1)?\n");
		printf("您想放弃读盘（选0）\n\t还是重新选择目标文件读盘？\n\n");
	    while(1)
	    {
		    gets (order);
	        while(order[0]=='\0')
		        gets(order);
		    l_order=strlen(order);
		    ch=order[0];
		    for(i=0;i<l_order-1;i++)  //截取串中第一个不为空格的字符
		    {
			    if(order[i]==' ')
			    {
				    ch=order[i+1];
			    }
			    else
				    break;
		    }
		    if((ch=='0')||(ch=='1'))
		    {
			   break;
		    }
		    else
		    {
			    printf("\nPlease input an order(1/0) that could be understood.\n");
				printf("请输入符合规定的指令字符(1或者0)");
		    }
	    }
	    if(ch == '1')
	    {
		    fp=NULL;
		    goto read;
	    }
		else
			return;
	}
	return ;
}

int human[SCALE][SCALE]={{0}};
int computer[SCALE][SCALE]={{0}};    //能否落子

int comp_x,comp_y;       //最新落子点

void change_color(int , int , char, char [SCALE][SCALE]);
int judge_put(int ,int , char, char [SCALE][SCALE]);
void possible();
int mobility(char);
int start();
int end_judge();

//每个位置的权重
int const_value[SCALE][SCALE];  //固有部分
int change_value[SCALE][SCALE];  //时时在变

void put_value()
{
	int i,j;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			const_value[i][j]=1;
	const_value[0][0]=const_value[0][SCALE-1]
	=const_value[SCALE-1][0]=const_value[SCALE-1][SCALE-1]=800;
	for(i=1;i<SCALE-1;i++)
	{
		const_value[0][i]=100;
		const_value[SCALE-1][i]=100;
		const_value[i][0]=100;
		const_value[i][SCALE-1]=100;
	}
	const_value[0][1]=const_value[1][0]=const_value[1][1]=-200;
	const_value[0][SCALE-2]=const_value[1][SCALE-2]=const_value[1][SCALE-1]=-200;
	const_value[SCALE-1][1]=const_value[SCALE-2][0]=const_value[SCALE-2][1]=-200;
	const_value[SCALE-1][SCALE-2]=const_value[SCALE-2][SCALE-1]=const_value[SCALE-2][SCALE-2]=-200;

	const_value[0][3]=const_value[0][4]=-100;
	const_value[SCALE-1][3]=const_value[SCALE-1][4]=-100;
	const_value[3][0]=const_value[4][0]=-10;
	const_value[3][SCALE-1]=const_value[4][SCALE-1]=-100;
	return;
}

void new_board()
{
	int i,j;
    for(i=0;i<SCALE;i++)
        for(j=0;j<SCALE;j++)
        {
            board[i][j] = EMPTY;
        }

	// 初始棋局
	board[SCALE/2-1][SCALE/2-1] = WHITE;
	board[SCALE/2][SCALE/2] = WHITE;
	board[SCALE/2-1][SCALE/2] = BLACK;
	board[SCALE/2][SCALE/2-1] = BLACK;
	return;
}


// 画棋盘
void DrawChess(int ,int );
void DrawBoard()
{  
    int i,j;  
	int n=SCALE;
	// 打印第一行的标记 A B C D E F G H
	printf("\n    While playing, feel free to ask for help.\n");
	printf("\n    For any request ,input a negative integer at any time when it's your turn\n");
	printf("\n    游戏过程中有任何需求，只需输入一个负整数即可获得帮助\n\n");
	printf("\t\t      A");					// 第一个要单独打印
	for(i=1;i<n;i++) {					// 打印其他的横坐标信息
		printf("%4c",char(i+int('A')));
	}
	printf("\n");

	// 打印第一行棋盘最顶层
	printf("\t\t   ┌—");					// 左上角第一个要单独打印
	for(i=1;i<n;i++) {					// 打印其他的格线
		printf("┬—");
	}
	printf("┐\n");						// 打印右上角 


	// 打印从第0行到第n-2行的棋盘，每一行数据以及分割线
    for(i=0;i<n-1;i++)
    { 
		// 下面打印每一行，每行最左最右都有行坐标；每行都先打数据，再打一行纯的横线分割
		printf("\t\t%2d ", i);				// 先打的i行标记（纵坐标）
	
		// 打印第j列的实质内容，即从第0到第n-2列的数据以及分割线
		for(j=0;j<n;j++) {
			printf("│");				// 打印分割符号
			//DrawChess(i,j,board);		// 打印这个棋盘符号
			DrawChess(i,j);
		}
		printf("│%2d\n",i);			// 打印最后一列的墙，以及坐标

		// 打一行纯的横线分割
		printf("\t\t   ├");				// 打印中间分割墙的左边界
		for(j=0;j<n-1;j++) {			// 打印分割墙
			 printf("—┼");
		}
         printf("—┤\n");				// 打印中间分割墙的右边界
    }

	// 打印最后的那一行
	printf("\t\t%2d ", n-1);				// 先打印最后一行标记（纵坐标）
	// 打印第j列
	for(j=0;j<n;j++) {
			printf("│");				// 打印分割符号
			DrawChess(n-1,j);
	}
	printf("│%2d\n",n-1);				// 打印最后一列的墙，以及坐标
	
	
	// 打印最后一行棋盘的最底层
	printf("\t\t   └—");					// 左下角第一个要单独打印
	for(i=0;i<n-1;i++) {				// 打印其他的最下一行格线
		printf("┴—");
	}
	printf("┘\n");						// 打印右上角


	// 打印最后一行的座标 A B C D E F G H
	printf("\t\t      A");					// 第一个要单独打印
	for(i=1;i<n;i++) {					// 打印其他的横坐标信息
		printf("%4c",char(i+int('A')));
	}
	printf("\n");	
}

// 打印i,j位的图像
void DrawChess(int i, int j)
{
	switch(board[i][j]) {
		case EMPTY:		printf("  ");	// 空格子
						break;
		case BLACK:		printf("●");	// 黑棋
						break;
		case WHITE:	    printf("◎");	// 白棋
						break;
		default:		break;
	}
	return;
}
int Choices()
{
	printf("\n");
	printf("╔══════════════════════════════╗\n");
	printf("║                                                            ║\n"); 
	printf("║     OTHELLO GAME PROGRAMED BY PATRICIA.XIAO                ║\n"); 
	printf("║     黑白棋人机对战小游戏 肖之屏出品                        ║\n"); 
	printf("║                                                            ║\n"); 
	printf("║     Welcome to play Othello with me~                       ║\n"); 
	printf("║     Enjoy yourself and have a great time~~~                ║\n"); 
	printf("║     欢迎使用简易版黑白棋对战小程序 祝您玩得愉快 ~~~        ║\n"); 
	printf("║                                                            ║\n");
	printf("║            1.RULES        规则说明                         ║\n"); 
	printf("║            2.A NEW START  新开始                           ║\n"); 
	printf("║            3.LOAD         读盘                             ║\n"); 
	printf("║            4.SAVE         存盘                             ║\n"); 
	printf("║            0.END & EXIT   结束                             ║\n");
	printf("║                                                            ║\n");
	printf("╚══════════════════════════════╝\n"); 
	printf("\tSelet a choice from the above, please:\n");
	printf("\t请输入0-4中的某个数字：\n\n"); 
	char choice[60];
	char c;
	int l;
	int i;
	int a;
	while(1)
	{
		gets(choice);
		while(choice[0]=='\0')
			gets(choice);
		l=strlen(choice); 
		c=choice[0];
		for(i=0;i<l-1;i++)
		{
			if(choice[i]==' ')
				c=choice[i+1];
			else
				break;
		}
		if ((c < '0')||(c >'4'))
		{
			printf("\tPlease input a number between 0 and 4 according to the request, thanks a lot:\n");
			printf("\t请按要求输入0-4中的某个数字，谢谢合作：\n\n"); 
		}
		else
			break;
	}
	a=c-'0';
	return a;
}

int print_rule();

int main()
{
	int order_before;
	int order_after;
	put_value();
	new_board();
	game:
	while(1)
	{
		system("CLS");  //把上一局清干净
		order_before=Choices();
		if(order_before==0)
			break;
		if(order_before==1)
		{
			system("CLS");
			order_before=print_rule();
			if(order_before==1)
			    continue;
			else
				break;
		}
		if(order_before==4)
		{
			SaveBoard();
			system("PAUSE");
			continue;
		}
		if(order_before==2)
			new_board();
		if(order_before==3)
			ReadBoard(); 
		if((order_before==2)||(order_before==3))
		{
			system("CLS");
			DrawBoard();
			printf("\nPlease choose your side(1/2):\n");
			printf("请选择您的棋子(1/2)：\n\n");
			printf("1.black●(go first)\n  黑子（先手）\n\n2.white◎(go second)\n  白子（后手）\n\n");
			char before[60],o_b;
			int l_b,i;
			while(1)
			{
				printf("\nYour choice:\n");
				printf("请输入您的选择：\n");
				gets(before);
				while(before[0]=='\0')
					gets(before);
				l_b=strlen(before);
				o_b=before[0];
				for(i=0;i<l_b-1;i++)
				{
					if(before[i]==' ')
						o_b=before[i+1];
					else
						break;
				}
				if((o_b=='1')||(o_b=='2'))
					break;
				else
				{
					printf("\nPlease input correctly:\n");
					printf("请正确输入您的选择：\n\n");
				}
			}
			order_before=o_b-'1'+1;
			system("CLS");
			if(order_before==1)
			{
				HUMAN=BLACK;
				COMPUTER=WHITE;
				possible();
	            printf("\nAs you chose the black side, you go first\n");
				printf("黑子先手，阁下先请:\n\n");
			}
			else
			{
				HUMAN=WHITE;
				COMPUTER=BLACK;
				possible();
				printf("\nAs you chose the white side, I go first\n");
				printf("白子后手，承蒙阁下谦让，在下不客气了：\n\n");
				srand((unsigned)time(NULL));  //初始化
				int a=rand();   //随机数
				int can[5][2];
				int num=0;
				int i,j;
				for(i=0;i<SCALE;i++)
				{
					for(j=0;j<SCALE;j++)
					{
						if(computer[i][j])
						{
							can[num][0]=i;
							can[num][1]=j;
							num++;
						}
					}
				}
				change_color(can[a%num][0],can[a%num][1],COMPUTER,board);
				possible();
			}
	        DrawBoard();
	        order_after=start();
		    if(order_after==1)
			    continue;
		    if(order_after==3)
			    break;
		    if(order_after==2)
		    {
			    printf("And then?\n");
				printf("阁下还有什么吩咐？\n\n");
			    printf("1.new game\n  新游戏\n\n2.leave now\n  退出\n\n");
				char after[60];
				int l_after;
				char o_a;
				int k;
			    while(1)
				{
					gets(after);
					while(after[0]=='\0')
						gets(after);
					l_after=strlen(after);
					o_a=after[0];
					for(k=0;k<l_after-1;k++)
					{
						if(after[k]==' ')
							o_a=after[k+1];
						else
							break;
					}
					if((o_a=='1')||(o_a=='2'))
						break;
					else
					{
						printf("\nplease input correctly:\n");
						printf("请正确输入：\n\n");
					}
				}
				order_after=o_a-'1'+1;
			    if(order_after==2)
				    break;
			    else
				    continue;
		    }
		}
	}
	char yes_or_no[60],ch;
	int l,i;
	printf("Do you really mean to leave?\n");
	printf("您是当真想退出的么？\n\n");
	printf("1.Yes  是的\n2.No  不是\n\n");
	while(1)
	{
		printf("Your choice,please:\n");
		printf("请输入您的选择：\n\n");
	    gets(yes_or_no);
		while(yes_or_no[0]=='\0')
			gets(yes_or_no);
		l=strlen(yes_or_no);
		ch=yes_or_no[0];
		for(i=0;i<l-1;i++)
		{
			if(yes_or_no[i]==' ')
				ch=yes_or_no[i+1];
			else
				break;
		}
		if((ch=='1')||(ch=='2'))
			break;
		else
		{
			printf("\nPlease input correctly:\n");
			printf("请正确输入您的选择\n");
		}
	}
	if(order_after==2)
		goto game;
	return 0;
}

int print_rule()
{
	int code_order;
		printf("╔═════════════════════════════════════╗\n");
		printf("║                                                                          ║\n");
		printf("║ BREIF INTRODUCTION OF THE BACKGROUND                                     ║\n");
		printf("║ 黑白棋游背景知识戏简单介绍：                                             ║\n");
		printf("║                                                                          ║\n");
		printf("║ Othello, which is widely known as 'Black and White' Chess in China,      ║\n");
		printf("║ Has several other names such as Reversi, Apple Chess, and so on.         ║\n");
		printf("║ 黑白棋，又叫反棋（Reversi）、奥赛罗棋（Othello）、苹果棋或翻转棋。       ║\n");
		printf("║                                                                          ║\n");
		printf("║ It is a popular game in both Japan and the Western world.                ║\n");
		printf("║ 黑白棋在西方和日本很流行。                                               ║\n");
		printf("║                                                                          ║\n");
		printf("║ During the game, players change the color of chesses on the board.       ║\n");
		printf("║ When both sides have no legal place to put chess anymore,                ║\n");
		printf("║ game is over, and the side with more chesses on the board is the winner. ║\n");
		printf("║ 游戏通过相互翻转对方的棋子，最后以棋盘上谁的棋子多来判断胜负。           ║\n");
		printf("║                                                                          ║\n");
		printf("║ The rules are so simple that everyone could learn to play it.            ║\n");
		printf("║ However, it is fairly difficult to play it well,                         ║\n");
		printf("║ since changes take place in the game are complex and hard to predict.    ║\n");
		printf("║ 它的游戏规则简单，因此上手很容易，但是它的变化又非常复杂。               ║\n");
		printf("║                                                                          ║\n");
		printf("║ As a saying goes, it takes minutes to learn it,                          ║\n");
		printf("║ but to be good on it cost you a whole lifetime.                          ║\n");
		printf("║ 有一种说法是：只需要几分钟学会它，却需要一生的时间去精通它。             ║\n");
		printf("║                                                                          ║\n");
		printf("║═════════════════════════════════════║\n");
		printf("║                                                                          ║\n");
		printf("║ RULES IN DETAIL:                                                         ║\n");
		printf("║ 规则明细：                                                               ║\n");
		printf("║                                                                          ║\n");
		printf("║                                                                          ║\n");
		printf("║ Put your chess(A) in an empty square on the board,                       ║\n");
		printf("║ 把自己颜色的棋子放在棋盘的空格上，                                       ║\n");
		printf("║                                                                          ║\n");
		printf("║ on the eight directions of A,                                            ║\n");
		printf("║ if there is a chess of your side(B),                                     ║\n");
		printf("║ between A and B there are only chesses of the opposite side,             ║\n");
		printf("║ no empty squares and no other chesses of your side in between,           ║\n");
		printf("║ 若自己放下的棋子在横、竖、斜八个方向内有一个自己的棋子，                 ║\n");
		printf("║                                                                          ║\n");
		printf("║ and all the opposite chesses between A and B will be changed into        ║\n");
		printf("║ the color of your side.                                                  ║\n");
		printf("║ 则被夹在中间的全部翻转会成为自己的棋子。                                 ║\n");
		printf("║                                                                          ║\n");
		printf("║ Also, only those places that can make the color of chesses change        ║\n");
		printf("║ are places legal for you to place your chess.                            ║\n");
		printf("║ 并且，只有在可以翻转棋子的地方才可以下子。                               ║\n");
		printf("║                                                                          ║\n");
		printf("║ When it's your turn but you have no legal place to put chess on,         ║\n");
		printf("║ skip to your opponent's turn.                                            ║\n");
		printf("║ Otherwise you have to put a chess.                                       ║\n");
		printf("║ 当一方选手无处落子时，直接轮到下一方。                                   ║\n");
		printf("║ 其他原因均不得放弃落子。                                                 ║\n");
		printf("║                                                                          ║\n");
		printf("║ When both sides have no legal place to put chess anymore,                ║\n");
		printf("║ game is over, and the side with more chesses on the board is the winner. ║\n");
		printf("║ 到双方都不能落子时，比赛结束。                                           ║\n");
		printf("║ 剩余子数最多的是赢家。                                                   ║\n");
		printf("║                                                                          ║\n");
		printf("╚═════════════════════════════════════╝\n");
		printf("\tafter reading the introduction, what's your decision next?\n");
		printf("\n\t明白了的话请选择继续游戏，还是退出游戏：\n\n");
		printf("\n\t1.Continue 继续游戏\n\t2.Leave   退出游戏\n\n");
		printf("\tPlease input your choice:\n");
		printf("\t请输入您的选择：\n\n");
		char order[60],c;
		int i,l;
		while(1)
		{
			gets(order);
			while(order[0]=='\0')
				gets(order);
			c=order[0];
			l=strlen(order);
			for(i=0;i<l-1;i++)
			{
				if(order[i]==' ')
					c=order[i+1];
				else
					break;
			}
			if((c!='1')&&(c!='2'))
			{
				printf("\tPlease input correctly:\n");
				printf("\t请正确输入您的选择：\n\n");
			}
			else
				break;
		}
		code_order=c-'1'+1;
		return code_order;
}

//判断能否落子的函数
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)   //不能重叠放子
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)   //不能重叠放子
		return 0;
	//上
	if(i>0)
	{
		if(board[i-1][j]==against)
		{
			for(k=i-2;k>=0;k--)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
					return 1;
			}
		}
	}
	//下
	if(i<SCALE-1)
	{
		if(board[i+1][j]==against)
		{
			for(k=i+2;k<SCALE;k++)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
					return 1;
			}
		}
	}
	//左
	if(j>0)
	{
		if(board[i][j-1]==against)
		{
			for(k=j-2;k>=0;k--)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
					return 1;
			}
		}
	}
	//右
	if(j<SCALE-1)
	{
		if(board[i][j+1]==against)
		{
			for(k=j+2;k<SCALE;k++)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
					return 1;
			}
		}
	}
	//左上
	if((i>0)&&(j>0))
	{
		if(board[i-1][j-1]==against)
		{
			for(k=2;((i-k>=0)&&(j-k>=0));k++)
			{
				if(board[i-k][j-k]==EMPTY)
					break;
				if(board[i-k][j-k]==whoseturn)
					return 1;
			}
		}
	}
	//右上
	if((i>0)&&(j<SCALE-1))
	{
		if(board[i-1][j+1]==against)
		{
			for(k=2;((i-k>=0)&&(j+k<SCALE));k++)
			{
				if(board[i-k][j+k]==EMPTY)
					break;
				if(board[i-k][j+k]==whoseturn)
					return 1;
			}
		}
	}
	//左下
	if((i<SCALE-1)&&(j>0))
	{
		if(board[i+1][j-1]==against)
		{
			for(k=2;(i+k<SCALE)&&(j-k>=0);k++)
			{
				if(board[i+k][j-k]==EMPTY)
					break;
				if(board[i+k][j-k]==whoseturn)
					return 1;
			}
		}
	}
	//右下
	if((i<SCALE-1)&&(j<SCALE-1))
	{
		if(board[i+1][j+1]==against)
		{
			for(k=2;((i+k<SCALE)&&(j+k<SCALE));k++)
			{
				if(board[i+k][j+k]==EMPTY)
					break;
				if(board[i+k][j+k]==whoseturn)
					return 1;
			}
		}
	}
	return 0;
}


//更新能否落子信息的函数
void possible()
{
	int i, j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			human[i][j]=computer[i][j]=0;
			if(judge_put(i,j,HUMAN,board))
				human[i][j]=1;
			if(judge_put(i,j,COMPUTER,board))
				computer[i][j]=1;
		}
	}
}


//判断行动力的函数
int mobility(char this_side)
{
	int i,j;
	int n=0;
	if(this_side==COMPUTER)
	{
	    for(i=0;i<SCALE;i++)
		    for(j=0;j<SCALE;j++)
			    if(computer[i][j])
					n++;
	}
	else  //bug= =b没加大括号（我就是懒了一下，至于吗……）
	{
		for(i=0;i<SCALE;i++)
			for(j=0;j<SCALE;j++)
				if(human[i][j])
					n++;
	}
	return n;
}



//落子之后改变棋盘的函数
void change_color(int i, int j, char whoseturn,char board[SCALE][SCALE])
{
	int x,y;
	int k;
	char against;
	if(whoseturn==HUMAN)
		against=COMPUTER;
	else
		against=HUMAN;
	//上
	board[i][j]=whoseturn;
	if(i>0)
	{
		if(board[i-1][j]==against)
		{
			for(k=i-2;k>=0;k--)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
				{
					for(x=i-1;x>k;x--)
						board[x][j]=whoseturn;
					break;
				}
			}
		}
	}
	//下
	if(i<SCALE-1)
	{
		if(board[i+1][j]==against)
		{
			for(k=i+2;k<SCALE;k++)
			{
				if(board[k][j]==EMPTY)
					break;
				if(board[k][j]==whoseturn)
				{
					for(x=i+1;x<k;x++)
						board[x][j]=whoseturn;
					break;
				}
			}
		}
	}
	//左
	if(j>0)
	{
		if(board[i][j-1]==against)
		{
			for(k=j-2;k>=0;k--)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
				{
					for(x=j-1;x>k;x--)
						board[i][x]=whoseturn;
					break;
				}
			}
		}
	}
	//右
	if(j<SCALE-1)
	{
		if(board[i][j+1]==against)
		{
			for(k=j+2;k<SCALE;k++)
			{
				if(board[i][k]==EMPTY)
					break;
				if(board[i][k]==whoseturn)
				{
					for(x=j+1;x<k;x++)
						board[i][x]=whoseturn;
					break;
				}
			}
		}
	}
	//左上
	if((i>0)&&(j>0))
	{
		if(board[i-1][j-1]==against)
		{
			for(k=2;((i-k>=0)&&(j-k>=0));k++)
			{
				if(board[i-k][j-k]==EMPTY)
					break;
				if(board[i-k][j-k]==whoseturn)
				{
					for(x=i-1,y=j-1;x>i-k;x--,y--)
						board[x][y]=whoseturn;
					break;
				}
			}
		}
	}
	//右上
	if((i>0)&&(j<SCALE-1))
	{
		if(board[i-1][j+1]==against)
		{
			for(k=2;((i-k>=0)&&(j+k<SCALE));k++)
			{
				if(board[i-k][j+k]==EMPTY)
					break;
				if(board[i-k][j+k]==whoseturn)
				{
					for(x=i-1,y=j+1;x>i-k;x--,y++)
						board[x][y]=whoseturn;
					break;
				}
			}
		}
	}
	//左下
	if((i<SCALE-1)&&(j>0))
	{
		if(board[i+1][j-1]==against)
		{
			for(k=2;(i+k<SCALE)&&(j-k>=0);k++)
			{
				if(board[i+k][j-k]==EMPTY)
					break;
				if(board[i+k][j-k]==whoseturn)
				{
					for(x=i+1,y=j-1;x<i+k;x++,y--)
						board[x][y]=whoseturn;
					break;
				}
			}
		}
	}
	//右下
	if((i<SCALE-1)&&(j<SCALE-1))
	{
		if(board[i+1][j+1]==against)
		{
			for(k=2;((i+k<SCALE)&&(j+k<SCALE));k++)
			{
				if(board[i+k][j+k]==EMPTY)
					break;
				if(board[i+k][j+k]==whoseturn)
				{
					for(x=i+1,y=j+1;x<i+k;x++,y++)
						board[x][y]=whoseturn;
					break;
				}
			}
		}
	}
	return ;
}

int end_judge()
{
	int h=0,c=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==HUMAN)
				h++;
			else
				if(board[i][j]==COMPUTER)
					c++;
		}
	}
	printf("╔══\n");
	printf("║ \n");
	printf("\tThe final result: %d:%d\n",h,c);
	if(h>c)
		printf("\tyou win!\n");
	else
		if(h==c)
			printf("\ttie\n");
		else
			printf("\tyou lose~\n");
	printf("                                                  ║\n");
	printf("                                              ══╝\n\n\n");
	printf("╔══\n");
	printf("║ \n");
	printf("\n\n\t游戏结束~~~\n\t现在公布比赛结果：\n\n");
	printf("\t最终结果：\n\n\t\t您的子剩余%d个，计算机的子剩余%d个！\n\n",h,c);
	if(h<c)
	    printf("\t就结论来讲，很遗憾您输了！但是请别气馁，对手可是计算机，您已经很厉害啦！！\n");
	else
		if(h>c)
			printf("\t天哪，连计算机都下不赢您，您这是要逆天啊！给大神跪了！\n");
		else
		    printf("\t平手啊~~~难分胜负的说~~~不然再来一盘吧~~~\n");
    printf("                                                                              ║\n");
	printf("                                                                          ══╝\n");
	int a;
	char your_choice[60],ch;
	int l;
	printf("Your choice:\n现在您想：\n\n1.a new game\n  开始新一盘游戏\n\n2.save\n  存盘\n\n3.leave\n  退出\n\n");
	while(1)
	{
		printf("\nYour choice,please:\n");
		printf("请给出您的选择：\n\n");
		gets(your_choice);
		while(your_choice[0]=='\0')
			gets(your_choice);
		l=strlen(your_choice);
		ch=your_choice[0];
		for(i=0;i<l-1;i++)
		{
			if(your_choice[i]==' ')
				ch=your_choice[i+1];
			else
				break;
		}
		if((ch>='1')&&(ch<='3'))
			break;
		else
		{
			printf("\nPlease input correctly\n");
			printf("请正确输入\n\n");
		}
	}
	a=ch-'1'+1;
	return a;
}

//寻找最优点落棋的系列函数

//评定稳定系数的函数
int steady(int x, int y, char thisside, char board[SCALE][SCALE])
{
	char opposite;
	if(thisside==COMPUTER)
		opposite=HUMAN;
	else
		opposite=COMPUTER;
	int i,j;
	int k;
	int z=0;
	//横向
	for(j=y-1;j>=0;j--)
	{
		if(board[x][j]==EMPTY)
		{
			z--;
			for(k=y+1;k<SCALE;k++)
			{
				if(board[x][k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x][k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[x][j]==opposite)
		{
			z--;
			for(k=y+1;k<SCALE;k++)
			{
				if(board[x][k]==EMPTY)
				{
					z--;
					break;
				}
				if(board[x][k]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	//纵向
	for(i=x-1;i>=0;i--)
	{
		if(board[i][y]==EMPTY)
		{
			z--;
			for(k=x+1;k<SCALE;k++)
			{
				if(board[k][y]==EMPTY)
				{
					z++;
					break;
				}
				if(board[k][y]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][y]==opposite)
		{
			z--;
			for(k=x+1;k<SCALE;k++)
			{
				if(board[k][y]==EMPTY)
				{
					z--;
					break;
				}
				if(board[k][y]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	//左上右下
	for(i=x-1,j=y-1;(i>=0)&&(j>=0);i--,j--)
	{
		if(board[i][j]==EMPTY)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y+k<SCALE);k++)
			{
				if(board[x+k][y+k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y+k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][j]==opposite)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y+k<SCALE);k++)
			{
				if(board[x+k][y+k]==EMPTY)
				{
					z--;
					break;
				}
				if(board[x+k][y+k]==opposite)
				{
					z++;
					break;
				}
			}
			break;
		}
	}
	z++;
	//左下右上
	for(i=x-1,j=y+1;(i>=0)&&(j<SCALE);i--,j++)
	{
		if(board[i][j]==EMPTY)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y-k>=0);k++)
			{
				if(board[x+k][y-k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y-k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
		if(board[i][j]==opposite)
		{
			z--;
			for(k=1;(x+k<SCALE)&&(y-k>=0);k++)
			{
				if(board[x+k][y-k]==EMPTY)
				{
					z++;
					break;
				}
				if(board[x+k][y-k]==opposite)
				{
					z--;
					break;
				}
			}
			break;
		}
	}
	z++;
	return z;
}

void c_val(char board[SCALE][SCALE])
{
	int i,j;
	int n=SCALE-1;
	for(i=0;i<=n;i++)
		for(j=0;j<=n;j++)
			change_value[i][j]=0;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))  //上
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))   //下
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))  //左
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))  //右
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	//对方占了角的情况（粗略处理）
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1]=-300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1]=-300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1]=-300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2]=-300;
	//对方占边的策略变化粗处理
	for(i=1;i<SCALE-1;i++)
	{
		if(board[0][i]==HUMAN)
		{
			change_value[0][i-1]-=100;
			change_value[0][i+1]-=100;
		}
		if(board[SCALE-1][i]==HUMAN)
		{
			change_value[SCALE-1][i-1]-=100;
			change_value[SCALE-1][i+1]-=100;
		}
		if(board[i][0]==HUMAN)
		{
			change_value[i-1][0]=-100;
			change_value[i+1][0]=-100;
		}
		if(board[i][SCALE-1]==HUMAN)
		{
			change_value[i-1][SCALE-1]-=100;
			change_value[i+1][SCALE-1]-=100;
		}
	}
	return;
}
//估值函数
//给当前棋局估值

int temp_value(char board[SCALE][SCALE])
{
	c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				score+=steady(i,j,COMPUTER,board)*30;  //稳定性
				score+=(const_value[i][j]+change_value[i][j]);      //位置固有权重+可变权重
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //注意change_value全是以计算机角度为准的！
				}
			if(judge_put(i,j,COMPUTER,board))  //行动力
				score+=100;
			if(judge_put(i,j,HUMAN,board))
				score-=100;
		}
	}
	return score;
}

//加alpha-beta剪枝
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;  //分数
	int temp_x=0;
	int temp_y=0;
	char now;
	if(depth%2==0)
	{
		now=COMPUTER;
	}
	else
	{
		now=HUMAN;
	}
	if(depth==DEPTH)
		return temp_value(last_board);
	char temp_board[SCALE][SCALE];
	int i,j;
	int p,q;
	int a=0;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			temp_board[i][j]=last_board[i][j];
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(judge_put(i,j,now,last_board))  //可以落子之处
			{
				change_color(i,j,now,temp_board);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++)
						temp_board[p][q]=last_board[p][q]; //复原
				if(depth%2==0)   //该机器走
				{
					if(a==0)  //只是为了确保有解//当然这样也许可以说是不完全的αβ剪枝//因为我的minimax本来也不是很正统。
				    {
					    alpha=temp;    //好吧我没说真话，真话是我的评估函数好夸张能超出整型范围的。
						temp_x=i;
					    temp_y=j;
					    a++;
					    continue;  
				    }
				    if(alpha<temp)
				    {
					    alpha=temp;
						temp_x=i;
						temp_y=j;
				    }
					if(alpha>=beta)
					{
						if(depth==0)
						{
							comp_x=temp_x;
							comp_y=temp_y;    //看来，问题是处在这里啊？不爱用指针伤不起……
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)   //人的一方
					{
						beta=temp;
					}
					if(alpha>=beta)
						return beta;
				}
			}
		}
	}
	if(depth==0)
	{
		comp_x=temp_x;
		comp_y=temp_y;
	}
	if(now==COMPUTER)
	    return alpha;
	else
		return beta;
}

int start()
{
	int m_h,m_c;
	int steps=0;
	char now;
	int i,j;
	char ci,cj,c;  //落子
	char order_line[200];   //为了避免一些出bug的可能性用字符串读入
	int place_i;  //为了成功切分么，没办法的事
	int lo;   //order's length
	int k;
	while(1)
	{
		possible();
		m_h=mobility(HUMAN);
		m_c=mobility(COMPUTER);
		if((m_h==0)&&(m_c)==0)
			break;
		if(steps%2==0)
		{
			printf("your turn\n");
			printf("轮到阁下了\n\n");
			now=HUMAN;
		}
		else
		{
			printf("computer's turn\n");
			printf("轮到在下了\n\n");
			now=COMPUTER;
		}
		if(mobility(now)==0)
		{
			printf("impossible for this side to put chess,move on\n");
			printf("当前选手无子可下，请下一位先\n\n");
			steps++;
			continue;
		}
		if(now==HUMAN)   //该人下了
		{
			while(1)
			{
				printf("\nYour choice,please:\n");
				printf("请给出您的选择:\n\n");
				gets(order_line);
				while(order_line[0]=='\0')
					gets(order_line);
				ci=order_line[0];
				place_i=0;
				lo=strlen(order_line);
				for(k=0;k<lo-2;k++)
				{
					if((order_line[k]==' ')||(order_line[k]==',')||(order_line[k]=='/')||(order_line[k]=='.'))
					{
						place_i=k+1;
						ci=order_line[k+1];
					}
					else
						break;
				}
				cj=order_line[place_i+1];
				for(k=place_i+1;k<lo-1;k++)
				{
					if(order_line[k]==' ')
					{
						cj=order_line[k+1];
					}
					else
						break;
				}
				if(ci=='-')
					break;
				if((((ci>='a')&&(ci<='h'))||((ci>='A')&&(ci<='H')))&&(cj>='0')&&(cj<='7'))
				{
					c=ci; ci=cj; cj=c;
				}
				if(((cj>'h')||(cj<'a'))&&((cj>'H')||(cj<'A')))
				{
					printf("\nuse a correct letter, please");
					printf("\n请注意列标范围\n\n");
					continue;
				}
				if((ci>='0')&&(ci<='7'))
					break;
				printf("\nNotice the form requested, please. A number for the row and a letter for the rol.");
				printf("\nNo matter which is typed first and whether the letter is capitalized.");
				printf("\n请注意输入形式，请输入行标（数字），列标（字母），不拘大小写与先后顺序\n\n");
			}
			if(ci=='-')
			{
				printf("\nWhat's the order,sir?\n");
				printf("请问您的指令是？\n\n");
				printf("1.Continue   继续游戏\n");
				printf("2.Save       存盘\n");
				printf("3.Quit       退出\n");
				printf("4.Load       读盘\n");
				printf("5.Help       帮助\n\n");
				int order;
				int l;
				char sto[60],ord_num;
				while(1)
				{
					gets(sto);
					while(sto[0]=='\0')
						gets(sto);
					l=strlen(sto);
					ord_num=sto[0];
					for(k=0;k<l-1;k++)
					{
						if(sto[k]==' ')
						{
							ord_num=sto[k+1];
						}
						else
							break;
					}
					if((ord_num>='1')&&(ord_num<='5'))
					{
						break;
					}
					else
					{
						printf("\n请注意正确输入您的选择\n");
						printf("\nplease input correctly\n\n");
					}
				}
				order=ord_num-'1'+1;
				if(order==1)
					continue;
				if(order==2)
				{
					SaveBoard();
					continue;
				}
				if(order==3)
					return 1;
				if(order==4)
				{
					ReadBoard();
					printf("\nThe board now:\n");
					printf("当前的棋盘：\n\n");
					DrawBoard();
					printf("\nThe side you chose at the begining does not change,\n");
					printf("双方所持有的棋子种类依旧，\n\n");
					printf("\tyou           阁下的棋子   :");
					switch(HUMAN)
					{
					case BLACK: printf("●"); break;
					case WHITE: printf("◎"); break;
					default: break;
					}
					printf("\n");
					printf("\tthe computer  在下的棋子   :");
					switch(COMPUTER)
					{
					case BLACK: printf("●"); break;
					case WHITE: printf("◎"); break;
					default: break;
					}
					printf("\n\n");
					printf("Press any key to continue\n");
					system("PAUSE");
					continue;
				}
				if(order==5)
				{
					printf("Your possible choices are:\n");
					printf("以下是您当前可以选择的落子之处:\n\n");
					possible();
					for(i=0;i<SCALE;i++)
					{
						for(j=0;j<SCALE;j++)
						{
							if(human[i][j])   //人能落子
							{
								printf("\t%d %c\n",i,j+'A');
							}
						}
					}
					printf("Press any key to continue\n");
					system("PAUSE");
					continue;    //不这样的话就直接轮到计算机了让我情何以堪……
				}
			}
			i=ci-'0';
			if((cj>='a')&&(cj<='h'))
				j=cj-'a';
			else
				j=cj-'A';
		    if(!human[i][j])
		    {
			    printf("The site you chose (%d,%c) is unavailable for you\n",i,'A'+j);
				printf("抱歉，您不能在该处 (%d,%c) 落子的\n\n",i,'A'+j);
			    continue;
		    }
			printf("your choice is: %d %c\n",i,j+'A');
			printf("您的选择是：    %d %c\n",i,j+'A');
			change_color(i,j,now,board);
			system("CLS");
			DrawBoard();
			steps++;
		}
		else
		{
			find_position(0,board,-inf,inf);
			if(judge_put(comp_x,comp_y,COMPUTER,board)==0)  //debug//这是保险措施，省得死循环
			{
				printf("%lld\n",find_position(0,board,-inf,inf));
				printf("%d\n",inf);
				printf("system error\n");  
				printf("%d,%dis not available\n",comp_x,comp_y);
				printf("Please tell Patricia.Xiao in order to improve this program. Thanks a lot.\n");
				printf("系统出现故障，请及时通知出品人肖之屏，她会改进本程序的，谢谢您的反馈。\n\n");
				printf("press any letter or number to move back\n");
				printf("请键入任意字符返回\n\n");
				char cv;
				scanf("%c",&cv);
				return 0;
			}
			change_color(comp_x,comp_y,now,board);
			//system("CLS");   //这里不加清屏了，让你瞧瞧，咱家程序不作弊的，别冤枉好程序
			DrawBoard();
			printf("The last step is:%d %c\n",comp_x,comp_y+'A');
			printf("方才在下是在（%d,%c）落子的\n\n",comp_x,comp_y+'A');
			steps++;
		}
	}
	return end_judge();
}

