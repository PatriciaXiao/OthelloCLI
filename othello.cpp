
/*
 * othelloai.cpp
 *
 *  Created on: 2012-12-22
 *      Author: patriciaxiao
 */

// �ڰ�����ַ������
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <io.h>

#include <iostream>
using namespace std;

// ��λ����Ϊ0 �������µ�λ����Ϊ1 �������µ�λ����Ϊ2
#define EMPTY		'0'
#define BLACK		'1'
#define WHITE	    '2'
#define SCALE		 8						//�ڰ����ģһ���� 8 X 8
#define DEPTH        6                      //�������
#define FileNameLEN  60

#define inf          0xfffffff      //����ƺ�û����ʲô�ó���

char HUMAN;
char COMPUTER;

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];  //����֮ǰ�Ž��������õ�

void DrawBoard();

void SaveBoard()		// �����̱��浽�ļ���
{
	system("CLS");
	DrawBoard();
	int i, j, k = 0;
	FILE *fp = NULL;    //��ʼ˭Ҳ��ָ��
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE], ch;
	char order[100];
	int l_order;

	while(fp==NULL)
	{
		printf("Please input the name of the file in which the board to be saved\n");
		printf("( No more than 60 letters )");
		printf("�������ļ��� ( С��60�ַ� ) \n");
		gets(filename);
		
		while (access(filename, 0) == 0)   //�Ѿ�����
		{
			printf("The file named '%s' has already existed, would you like to rewrite the file?/n", filename);
			printf( "�ļ�'%s'�Ѿ������ˣ���Ҫ�������𣿣�Y/N/Q��\n\n", filename);
			printf( "\tY=yes(�ǵģ�����)\nN=no(�����ǣ���������)\nQ=quit saving(��������)\n\n");
			printf("Your choice:\n");
			printf("���������ѡ��\n\n");
			while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //��ȡ���е�һ����Ϊ�ո���ַ�
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
					printf("�밴�չ涨����ʽ���룬���д�Сд��\n\n");
				}
			}
			if (ch == 'Y' || ch == 'y')
				break;     //�Ǿ�����������ˣ������ء����˼Ҷ�˵�����ˣ�
			else
				if(ch == 'q' || ch == 'Q' )
					return;
			    else 
				{
					printf("\nYour new choice of the file's nams:\n");
					printf("�����������ļ�����\n\n");
					gets(filename);
					continue;   //��������Ҫ��ʲô���֡���
				}
		}

		err  = fopen_s( &fp, filename, "w+" );

		if( err == 0 )
		{
			printf("File '%s' has been opened successfully, waiting to save the board into it\n",filename);
			printf( "�ļ�'%s'�ɹ��򿪣��ȴ�д��\n", filename );
			system("PAUSE");
		}
		else 
		{
			printf("The effort we paid trying to open the file '%s' has failed, what returned is an error code %d",filename,err);
			printf( "�ļ�'%s'��ʧ�ܣ����ش����� %d \n\n", filename, err);
			printf("Would you like to input another file's name in order to save the board(Y/N)?");
			printf("\n���Ƿ�ѡ������������ȷ���ļ����Դ��̣���Y/N��\n\n");
			printf("Y: Retry saving and input a file name again\n  ���������ļ���\n\n");
			printf("N: Give up saving the board\n  ��������\n\n");
            while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //��ȡ���е�һ����Ϊ�ո���ַ�
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
					printf("\n�밴Ҫ������ѡ��лл����(y/n,��Сд����)");
				}
			}
		}
	} 

	if (fp == NULL)  //˵���Ƿ��������˳���
		return ;

	for(i=0;i<SCALE;i++)      //����
	{
		for(j=0;j<SCALE;j++) 
		{
			str[k++] = board[i][j];
		}
		str[k++]='\n';			// �ӻس�
	}

    fwrite(str,1,(SCALE+1)*SCALE,fp);	// ����ԭ��size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
							// ע�⣬���� str��û�б���"\0"�ַ�����������������    fwrite(str,1,strlen(str),fp);	
    fclose(fp);  //�ǵù��ϡ���

	return;
}

//�ж��Ƿ�Ϸ����̵�ϵ�к���

//�ݹ�ר��
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;    //���緵��
	if(!b[i][j])  //��ֵ����
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);   //��˷�����
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
	return;
}

//�ж�����
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
				else                 //���ڷǷ��ַ�
				{
					return -1;
				}
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
		{
			if(!temp[i][j])  //�������û�����ӣ�һ���Ƿ�
				return 0;
		}
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(temp[i][j])
				return 0;   //δ����һƬ��һ������
		}
	}
	return 1;
}

void ReadBoard()		// ���ļ��ж�����������
{
	int i, j, k = 0;
	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE], ch;  //�Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������
	char order[100];
	int l_order;
	int okay;
 read:
	ch='0';
	while(fp == NULL) 
	{
		printf("Please input a file's name from which you'd like to get the board\n");
		printf("(warning: no more than 60 letters)\n");
		printf("�������ļ��� ( С�� 60 �ַ� ) \n\n");
		gets(filename);
		err  = fopen_s( &fp, filename, "r" );// fp = fopen(filename,"r+") ;
		if( err == 0 )
		{
			printf("File '%s' has been opened successfully\n", filename);
			printf( "�ļ�'%s'�ɹ���\n\n", filename );
			break;
		}
		else 
		{
			if (err == ENOENT)
			{
				printf("Sorry, the file '%s' does not exist\n",filename);
			    printf( "�ļ�'%s'������\n\n", filename);
			}
			else
			{
				printf("Returned an error code %d, the file you chose could not open.\n", filename, err);
			    printf( "�ļ�'%s'��ʧ�ܣ����ش����� %d \n\n", filename, err);
			}
			printf("Reinput a file's name?(Y/N)\n");
			printf("���������ļ�����(Y/N)\n\n");
			printf("Y=yes ��\nN=no ��\n\n");
			while(1)
			{
			    gets(order);
			    while(order[0]=='\0')
				    gets(order);
			    l_order=strlen(order);
			    ch=order[0];
			    for(i=0;i<l_order-1;i++)  //��ȡ���е�һ����Ϊ�ո���ַ�
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
					printf("\n�밴Ҫ������ѡ��лл����(y/n,��Сд����)");
				}
			}
			if (ch == 'N' || ch == 'n')
				return ;
		}
	}          //�ļ������Զ�Ӧ��ĳ�ĵ�

	fread(str,1,SCALE*(SCALE+1),fp);	
	// ����ԭ��size_t fread(void *buffer, size_t size, size_t count, FILE *stream);��

	for (i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++) 
		{
			save_board[i][j] = str[k++];
		}
		k++;					// ȥ���س�
	}

    fclose(fp);  //�ù��͹غ�ϰ��

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
			printf("\n�Բ�������ָ�������������Ǵ�������̣�������ѡ��лл����\n\n");
		}
		else
		{
			printf("\nI'm sorry but there are some characters illegal in the file\n");
			printf("�Բ�������ָ�����ļ��д��ڱ���Ϸ�޷�ʶ��ķ��ţ��޷�������\n\n");
		}

	    printf("\nDo you want to give up reading the board (0) , \n\tor select another file to open (1)?\n");
		printf("����������̣�ѡ0��\n\t��������ѡ��Ŀ���ļ����̣�\n\n");
	    while(1)
	    {
		    gets (order);
	        while(order[0]=='\0')
		        gets(order);
		    l_order=strlen(order);
		    ch=order[0];
		    for(i=0;i<l_order-1;i++)  //��ȡ���е�һ����Ϊ�ո���ַ�
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
				printf("��������Ϲ涨��ָ���ַ�(1����0)");
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
int computer[SCALE][SCALE]={{0}};    //�ܷ�����

int comp_x,comp_y;       //�������ӵ�

void change_color(int , int , char, char [SCALE][SCALE]);
int judge_put(int ,int , char, char [SCALE][SCALE]);
void possible();
int mobility(char);
int start();
int end_judge();

//ÿ��λ�õ�Ȩ��
int const_value[SCALE][SCALE];  //���в���
int change_value[SCALE][SCALE];  //ʱʱ�ڱ�

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

	// ��ʼ���
	board[SCALE/2-1][SCALE/2-1] = WHITE;
	board[SCALE/2][SCALE/2] = WHITE;
	board[SCALE/2-1][SCALE/2] = BLACK;
	board[SCALE/2][SCALE/2-1] = BLACK;
	return;
}


// ������
void DrawChess(int ,int );
void DrawBoard()
{  
    int i,j;  
	int n=SCALE;
	// ��ӡ��һ�еı�� A B C D E F G H
	printf("\n    While playing, feel free to ask for help.\n");
	printf("\n    For any request ,input a negative integer at any time when it's your turn\n");
	printf("\n    ��Ϸ���������κ�����ֻ������һ�����������ɻ�ð���\n\n");
	printf("\t\t      A");					// ��һ��Ҫ������ӡ
	for(i=1;i<n;i++) {					// ��ӡ�����ĺ�������Ϣ
		printf("%4c",char(i+int('A')));
	}
	printf("\n");

	// ��ӡ��һ���������
	printf("\t\t   ����");					// ���Ͻǵ�һ��Ҫ������ӡ
	for(i=1;i<n;i++) {					// ��ӡ�����ĸ���
		printf("�С�");
	}
	printf("��\n");						// ��ӡ���Ͻ� 


	// ��ӡ�ӵ�0�е���n-2�е����̣�ÿһ�������Լ��ָ���
    for(i=0;i<n-1;i++)
    { 
		// �����ӡÿһ�У�ÿ���������Ҷ��������ꣻÿ�ж��ȴ����ݣ��ٴ�һ�д��ĺ��߷ָ�
		printf("\t\t%2d ", i);				// �ȴ��i�б�ǣ������꣩
	
		// ��ӡ��j�е�ʵ�����ݣ����ӵ�0����n-2�е������Լ��ָ���
		for(j=0;j<n;j++) {
			printf("��");				// ��ӡ�ָ����
			//DrawChess(i,j,board);		// ��ӡ������̷���
			DrawChess(i,j);
		}
		printf("��%2d\n",i);			// ��ӡ���һ�е�ǽ���Լ�����

		// ��һ�д��ĺ��߷ָ�
		printf("\t\t   ��");				// ��ӡ�м�ָ�ǽ����߽�
		for(j=0;j<n-1;j++) {			// ��ӡ�ָ�ǽ
			 printf("����");
		}
         printf("����\n");				// ��ӡ�м�ָ�ǽ���ұ߽�
    }

	// ��ӡ������һ��
	printf("\t\t%2d ", n-1);				// �ȴ�ӡ���һ�б�ǣ������꣩
	// ��ӡ��j��
	for(j=0;j<n;j++) {
			printf("��");				// ��ӡ�ָ����
			DrawChess(n-1,j);
	}
	printf("��%2d\n",n-1);				// ��ӡ���һ�е�ǽ���Լ�����
	
	
	// ��ӡ���һ�����̵���ײ�
	printf("\t\t   ����");					// ���½ǵ�һ��Ҫ������ӡ
	for(i=0;i<n-1;i++) {				// ��ӡ����������һ�и���
		printf("�ء�");
	}
	printf("��\n");						// ��ӡ���Ͻ�


	// ��ӡ���һ�е����� A B C D E F G H
	printf("\t\t      A");					// ��һ��Ҫ������ӡ
	for(i=1;i<n;i++) {					// ��ӡ�����ĺ�������Ϣ
		printf("%4c",char(i+int('A')));
	}
	printf("\n");	
}

// ��ӡi,jλ��ͼ��
void DrawChess(int i, int j)
{
	switch(board[i][j]) {
		case EMPTY:		printf("  ");	// �ո���
						break;
		case BLACK:		printf("��");	// ����
						break;
		case WHITE:	    printf("��");	// ����
						break;
		default:		break;
	}
	return;
}
int Choices()
{
	printf("\n");
	printf("�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
	printf("�U                                                            �U\n"); 
	printf("�U     OTHELLO GAME PROGRAMED BY PATRICIA.XIAO                �U\n"); 
	printf("�U     �ڰ����˻���սС��Ϸ Ф֮����Ʒ                        �U\n"); 
	printf("�U                                                            �U\n"); 
	printf("�U     Welcome to play Othello with me~                       �U\n"); 
	printf("�U     Enjoy yourself and have a great time~~~                �U\n"); 
	printf("�U     ��ӭʹ�ü��װ�ڰ����սС���� ף�������� ~~~        �U\n"); 
	printf("�U                                                            �U\n");
	printf("�U            1.RULES        ����˵��                         �U\n"); 
	printf("�U            2.A NEW START  �¿�ʼ                           �U\n"); 
	printf("�U            3.LOAD         ����                             �U\n"); 
	printf("�U            4.SAVE         ����                             �U\n"); 
	printf("�U            0.END & EXIT   ����                             �U\n");
	printf("�U                                                            �U\n");
	printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n"); 
	printf("\tSelet a choice from the above, please:\n");
	printf("\t������0-4�е�ĳ�����֣�\n\n"); 
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
			printf("\t�밴Ҫ������0-4�е�ĳ�����֣�лл������\n\n"); 
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
		system("CLS");  //����һ����ɾ�
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
			printf("��ѡ����������(1/2)��\n\n");
			printf("1.black��(go first)\n  ���ӣ����֣�\n\n2.white��(go second)\n  ���ӣ����֣�\n\n");
			char before[60],o_b;
			int l_b,i;
			while(1)
			{
				printf("\nYour choice:\n");
				printf("����������ѡ��\n");
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
					printf("����ȷ��������ѡ��\n\n");
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
				printf("�������֣���������:\n\n");
			}
			else
			{
				HUMAN=WHITE;
				COMPUTER=BLACK;
				possible();
				printf("\nAs you chose the white side, I go first\n");
				printf("���Ӻ��֣����ɸ���ǫ�ã����²������ˣ�\n\n");
				srand((unsigned)time(NULL));  //��ʼ��
				int a=rand();   //�����
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
				printf("���»���ʲô�Ը���\n\n");
			    printf("1.new game\n  ����Ϸ\n\n2.leave now\n  �˳�\n\n");
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
						printf("����ȷ���룺\n\n");
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
	printf("���ǵ������˳���ô��\n\n");
	printf("1.Yes  �ǵ�\n2.No  ����\n\n");
	while(1)
	{
		printf("Your choice,please:\n");
		printf("����������ѡ��\n\n");
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
			printf("����ȷ��������ѡ��\n");
		}
	}
	if(order_after==2)
		goto game;
	return 0;
}

int print_rule()
{
	int code_order;
		printf("�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
		printf("�U                                                                          �U\n");
		printf("�U BREIF INTRODUCTION OF THE BACKGROUND                                     �U\n");
		printf("�U �ڰ����α���֪ʶϷ�򵥽��ܣ�                                             �U\n");
		printf("�U                                                                          �U\n");
		printf("�U Othello, which is widely known as 'Black and White' Chess in China,      �U\n");
		printf("�U Has several other names such as Reversi, Apple Chess, and so on.         �U\n");
		printf("�U �ڰ��壬�ֽз��壨Reversi�����������壨Othello����ƻ�����ת�塣       �U\n");
		printf("�U                                                                          �U\n");
		printf("�U It is a popular game in both Japan and the Western world.                �U\n");
		printf("�U �ڰ������������ձ������С�                                               �U\n");
		printf("�U                                                                          �U\n");
		printf("�U During the game, players change the color of chesses on the board.       �U\n");
		printf("�U When both sides have no legal place to put chess anymore,                �U\n");
		printf("�U game is over, and the side with more chesses on the board is the winner. �U\n");
		printf("�U ��Ϸͨ���໥��ת�Է������ӣ������������˭�����Ӷ����ж�ʤ����           �U\n");
		printf("�U                                                                          �U\n");
		printf("�U The rules are so simple that everyone could learn to play it.            �U\n");
		printf("�U However, it is fairly difficult to play it well,                         �U\n");
		printf("�U since changes take place in the game are complex and hard to predict.    �U\n");
		printf("�U ������Ϸ����򵥣�������ֺ����ף��������ı仯�ַǳ����ӡ�               �U\n");
		printf("�U                                                                          �U\n");
		printf("�U As a saying goes, it takes minutes to learn it,                          �U\n");
		printf("�U but to be good on it cost you a whole lifetime.                          �U\n");
		printf("�U ��һ��˵���ǣ�ֻ��Ҫ������ѧ������ȴ��Ҫһ����ʱ��ȥ��ͨ����             �U\n");
		printf("�U                                                                          �U\n");
		printf("�U�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�U\n");
		printf("�U                                                                          �U\n");
		printf("�U RULES IN DETAIL:                                                         �U\n");
		printf("�U ������ϸ��                                                               �U\n");
		printf("�U                                                                          �U\n");
		printf("�U                                                                          �U\n");
		printf("�U Put your chess(A) in an empty square on the board,                       �U\n");
		printf("�U ���Լ���ɫ�����ӷ������̵Ŀո��ϣ�                                       �U\n");
		printf("�U                                                                          �U\n");
		printf("�U on the eight directions of A,                                            �U\n");
		printf("�U if there is a chess of your side(B),                                     �U\n");
		printf("�U between A and B there are only chesses of the opposite side,             �U\n");
		printf("�U no empty squares and no other chesses of your side in between,           �U\n");
		printf("�U ���Լ����µ������ںᡢ����б�˸���������һ���Լ������ӣ�                 �U\n");
		printf("�U                                                                          �U\n");
		printf("�U and all the opposite chesses between A and B will be changed into        �U\n");
		printf("�U the color of your side.                                                  �U\n");
		printf("�U �򱻼����м��ȫ����ת���Ϊ�Լ������ӡ�                                 �U\n");
		printf("�U                                                                          �U\n");
		printf("�U Also, only those places that can make the color of chesses change        �U\n");
		printf("�U are places legal for you to place your chess.                            �U\n");
		printf("�U ���ң�ֻ���ڿ��Է�ת���ӵĵط��ſ������ӡ�                               �U\n");
		printf("�U                                                                          �U\n");
		printf("�U When it's your turn but you have no legal place to put chess on,         �U\n");
		printf("�U skip to your opponent's turn.                                            �U\n");
		printf("�U Otherwise you have to put a chess.                                       �U\n");
		printf("�U ��һ��ѡ���޴�����ʱ��ֱ���ֵ���һ����                                   �U\n");
		printf("�U ����ԭ������÷������ӡ�                                                 �U\n");
		printf("�U                                                                          �U\n");
		printf("�U When both sides have no legal place to put chess anymore,                �U\n");
		printf("�U game is over, and the side with more chesses on the board is the winner. �U\n");
		printf("�U ��˫������������ʱ������������                                           �U\n");
		printf("�U ʣ������������Ӯ�ҡ�                                                   �U\n");
		printf("�U                                                                          �U\n");
		printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n");
		printf("\tafter reading the introduction, what's your decision next?\n");
		printf("\n\t�����˵Ļ���ѡ�������Ϸ�������˳���Ϸ��\n\n");
		printf("\n\t1.Continue ������Ϸ\n\t2.Leave   �˳���Ϸ\n\n");
		printf("\tPlease input your choice:\n");
		printf("\t����������ѡ��\n\n");
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
				printf("\t����ȷ��������ѡ��\n\n");
			}
			else
				break;
		}
		code_order=c-'1'+1;
		return code_order;
}

//�ж��ܷ����ӵĺ���
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)   //�����ص�����
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)   //�����ص�����
		return 0;
	//��
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
	//��
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
	//��
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
	//��
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
	//����
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
	//����
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
	//����
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
	//����
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


//�����ܷ�������Ϣ�ĺ���
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


//�ж��ж����ĺ���
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
	else  //bug= =bû�Ӵ����ţ��Ҿ�������һ�£������𡭡���
	{
		for(i=0;i<SCALE;i++)
			for(j=0;j<SCALE;j++)
				if(human[i][j])
					n++;
	}
	return n;
}



//����֮��ı����̵ĺ���
void change_color(int i, int j, char whoseturn,char board[SCALE][SCALE])
{
	int x,y;
	int k;
	char against;
	if(whoseturn==HUMAN)
		against=COMPUTER;
	else
		against=HUMAN;
	//��
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
	//��
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
	//��
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
	//��
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
	//����
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
	//����
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
	//����
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
	//����
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
	printf("�X�T�T\n");
	printf("�U \n");
	printf("\tThe final result: %d:%d\n",h,c);
	if(h>c)
		printf("\tyou win!\n");
	else
		if(h==c)
			printf("\ttie\n");
		else
			printf("\tyou lose~\n");
	printf("                                                  �U\n");
	printf("                                              �T�T�a\n\n\n");
	printf("�X�T�T\n");
	printf("�U \n");
	printf("\n\n\t��Ϸ����~~~\n\t���ڹ������������\n\n");
	printf("\t���ս����\n\n\t\t������ʣ��%d�������������ʣ��%d����\n\n",h,c);
	if(h<c)
	    printf("\t�ͽ������������ź������ˣ�����������٣����ֿ��Ǽ���������Ѿ�������������\n");
	else
		if(h>c)
			printf("\t���ģ�����������²�Ӯ����������Ҫ���찡����������ˣ�\n");
		else
		    printf("\tƽ�ְ�~~~�ѷ�ʤ����˵~~~��Ȼ����һ�̰�~~~\n");
    printf("                                                                              �U\n");
	printf("                                                                          �T�T�a\n");
	int a;
	char your_choice[60],ch;
	int l;
	printf("Your choice:\n�������룺\n\n1.a new game\n  ��ʼ��һ����Ϸ\n\n2.save\n  ����\n\n3.leave\n  �˳�\n\n");
	while(1)
	{
		printf("\nYour choice,please:\n");
		printf("���������ѡ��\n\n");
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
			printf("����ȷ����\n\n");
		}
	}
	a=ch-'1'+1;
	return a;
}

//Ѱ�����ŵ������ϵ�к���

//�����ȶ�ϵ���ĺ���
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
	//����
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
	//����
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
	//��������
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
	//��������
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
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))  //��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))   //��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))  //��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))  //��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	//�Է�ռ�˽ǵ���������Դ���
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1]=-300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1]=-300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1]=-300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2]=-300;
	//�Է�ռ�ߵĲ��Ա仯�ִ���
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
//��ֵ����
//����ǰ��ֹ�ֵ

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
				score+=steady(i,j,COMPUTER,board)*30;  //�ȶ���
				score+=(const_value[i][j]+change_value[i][j]);      //λ�ù���Ȩ��+�ɱ�Ȩ��
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //ע��change_valueȫ���Լ�����Ƕ�Ϊ׼�ģ�
				}
			if(judge_put(i,j,COMPUTER,board))  //�ж���
				score+=100;
			if(judge_put(i,j,HUMAN,board))
				score-=100;
		}
	}
	return score;
}

//��alpha-beta��֦
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;  //����
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
			if(judge_put(i,j,now,last_board))  //��������֮��
			{
				change_color(i,j,now,temp_board);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++)
						temp_board[p][q]=last_board[p][q]; //��ԭ
				if(depth%2==0)   //�û�����
				{
					if(a==0)  //ֻ��Ϊ��ȷ���н�//��Ȼ����Ҳ�����˵�ǲ���ȫ�Ħ��¼�֦//��Ϊ�ҵ�minimax����Ҳ���Ǻ���ͳ��
				    {
					    alpha=temp;    //�ð���û˵�滰���滰���ҵ����������ÿ����ܳ������ͷ�Χ�ġ�
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
							comp_y=temp_y;    //�����������Ǵ������ﰡ��������ָ���˲��𡭡�
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)   //�˵�һ��
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
	char ci,cj,c;  //����
	char order_line[200];   //Ϊ�˱���һЩ��bug�Ŀ��������ַ�������
	int place_i;  //Ϊ�˳ɹ��з�ô��û�취����
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
			printf("�ֵ�������\n\n");
			now=HUMAN;
		}
		else
		{
			printf("computer's turn\n");
			printf("�ֵ�������\n\n");
			now=COMPUTER;
		}
		if(mobility(now)==0)
		{
			printf("impossible for this side to put chess,move on\n");
			printf("��ǰѡ�����ӿ��£�����һλ��\n\n");
			steps++;
			continue;
		}
		if(now==HUMAN)   //��������
		{
			while(1)
			{
				printf("\nYour choice,please:\n");
				printf("���������ѡ��:\n\n");
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
					printf("\n��ע���б귶Χ\n\n");
					continue;
				}
				if((ci>='0')&&(ci<='7'))
					break;
				printf("\nNotice the form requested, please. A number for the row and a letter for the rol.");
				printf("\nNo matter which is typed first and whether the letter is capitalized.");
				printf("\n��ע��������ʽ���������б꣨���֣����б꣨��ĸ�������д�Сд���Ⱥ�˳��\n\n");
			}
			if(ci=='-')
			{
				printf("\nWhat's the order,sir?\n");
				printf("��������ָ���ǣ�\n\n");
				printf("1.Continue   ������Ϸ\n");
				printf("2.Save       ����\n");
				printf("3.Quit       �˳�\n");
				printf("4.Load       ����\n");
				printf("5.Help       ����\n\n");
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
						printf("\n��ע����ȷ��������ѡ��\n");
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
					printf("��ǰ�����̣�\n\n");
					DrawBoard();
					printf("\nThe side you chose at the begining does not change,\n");
					printf("˫�������е������������ɣ�\n\n");
					printf("\tyou           ���µ�����   :");
					switch(HUMAN)
					{
					case BLACK: printf("��"); break;
					case WHITE: printf("��"); break;
					default: break;
					}
					printf("\n");
					printf("\tthe computer  ���µ�����   :");
					switch(COMPUTER)
					{
					case BLACK: printf("��"); break;
					case WHITE: printf("��"); break;
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
					printf("����������ǰ����ѡ�������֮��:\n\n");
					possible();
					for(i=0;i<SCALE;i++)
					{
						for(j=0;j<SCALE;j++)
						{
							if(human[i][j])   //��������
							{
								printf("\t%d %c\n",i,j+'A');
							}
						}
					}
					printf("Press any key to continue\n");
					system("PAUSE");
					continue;    //�������Ļ���ֱ���ֵ����������������Կ�����
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
				printf("��Ǹ���������ڸô� (%d,%c) ���ӵ�\n\n",i,'A'+j);
			    continue;
		    }
			printf("your choice is: %d %c\n",i,j+'A');
			printf("����ѡ���ǣ�    %d %c\n",i,j+'A');
			change_color(i,j,now,board);
			system("CLS");
			DrawBoard();
			steps++;
		}
		else
		{
			find_position(0,board,-inf,inf);
			if(judge_put(comp_x,comp_y,COMPUTER,board)==0)  //debug//���Ǳ��մ�ʩ��ʡ����ѭ��
			{
				printf("%lld\n",find_position(0,board,-inf,inf));
				printf("%d\n",inf);
				printf("system error\n");  
				printf("%d,%dis not available\n",comp_x,comp_y);
				printf("Please tell Patricia.Xiao in order to improve this program. Thanks a lot.\n");
				printf("ϵͳ���ֹ��ϣ��뼰ʱ֪ͨ��Ʒ��Ф֮��������Ľ�������ģ�лл���ķ�����\n\n");
				printf("press any letter or number to move back\n");
				printf("����������ַ�����\n\n");
				char cv;
				scanf("%c",&cv);
				return 0;
			}
			change_color(comp_x,comp_y,now,board);
			//system("CLS");   //���ﲻ�������ˣ��������ƣ��ۼҳ������׵ģ���ԩ���ó���
			DrawBoard();
			printf("The last step is:%d %c\n",comp_x,comp_y+'A');
			printf("�����������ڣ�%d,%c�����ӵ�\n\n",comp_x,comp_y+'A');
			steps++;
		}
	}
	return end_judge();
}

