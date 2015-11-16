
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
#define SCALE		 8								//�ڰ����ģһ���� 8 X 8

#define LOW			1								// �ͼ�������̰�ķ�
#define MIDIUM		3								// �м�������������
#define HIGH		5								// �߼������������

#define Menu_NONE		10							// ������ѡ��
#define Menu1_Rule		1							// ����
#define Menu1_New		2							// д��
#define Menu1_Read		3							// ����
#define Menu1_Save		4							// д��
#define Menu1_End		0							// �˳�

#define Menu2_Help		1							// ����
#define Menu2_Continue	2							// д��
#define Menu2_Read		3							// ����
#define Menu2_Save		4							// д��
#define Menu2_Back		0							// �˻�

#define Menu_Level		7							// ����

#define FileNameLEN  60

#define inf          0xfffffff						// ����ƺ�û����ʲô�ó���

char HUMAN = BLACK;									// ����Ϊ�˴�����̵Ļ����bug
char COMPUTER = WHITE;
int  DEPTH = HIGH;									// �������

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];						// ����֮ǰ�Ž��������õ�

#define  IllegalChar		1						// �ļ����зǷ��ַ� 
#define  NoCentralChess		2						// �м�û������
#define  NotConnected		3						// δ����һƬ
#define  GoodBoard			0						// ��������

int  human[SCALE][SCALE]={{0}};
int  computer[SCALE][SCALE]={{0}};					// �ܷ�����

int  comp_x,comp_y;									// �������ӵ�

void change_color(int , int , char, char [SCALE][SCALE]);
int  judge_put(int ,int , char, char [SCALE][SCALE]);
void possible();
int  mobility(char);
void end_judge();

//ÿ��λ�õ�Ȩ��
int const_value[SCALE][SCALE];						// ���в���
int change_value[SCALE][SCALE];						// ʱʱ�ڱ�

void DrawBoard();
int  print_rule();

void Level( );										// ѡ���������

int  judge_put(int ,int , char, char [SCALE][SCALE]);
void possible( );
int  mobility(char);
long long find_position(int, char[SCALE][SCALE], long long, long long);

void put_value( );									// �����̸�Ȩ��

void new_board();									// ���� 2 x 2 ����ĳ�ʼ���
int board_check(char [SCALE][SCALE]);				// ����������̵ĺϷ���
int  ReadBoard( );									// ���������
int  SaveBoard( );									// ����ִ���
void getchoice(char &, int, char []);				// ��ȡѡ��ֵ
void DrawBoard( );									// �������еĺڰ�����
void Current_Color( );								// ��ǰ��������˵�ִ����ɫ
void PrintScore( );									// ��ӡ��Һͼ�����ĵ÷�
void choose_Color( );								// ѡ��������ɫ����ɫ���ߣ�һ�㶼ѡ����
void start( );										// �������Ҫ���ƺ���, void Ҳ����
void level( );										// ѡ������
void Welcome_Info( );

// �ж������Ƿ��ʼ����
int Is_initialboard( )
{
	int i,j;
	int temp[SCALE][SCALE];

	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++) 
			temp[i][j] = board[i][j];

	if ( temp[SCALE/2-1][SCALE/2-1] != WHITE )
		return false;
	else temp[SCALE/2-1][SCALE/2-1] = EMPTY ;
	if ( temp[SCALE/2][SCALE/2] != WHITE )
		return false;
	else temp[SCALE/2][SCALE/2] = EMPTY ;
	if ( temp[SCALE/2-1][SCALE/2] != BLACK )
		return false;
	else temp[SCALE/2-1][SCALE/2] = EMPTY ;
	if (temp[SCALE/2][SCALE/2-1] != BLACK )
		return false;
	else temp[SCALE/2][SCALE/2-1] = EMPTY ;

	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++) 
			if ( temp[i][j] != EMPTY )
					return false;

	return true;
}

int main()
{
	char choice;
	int  menu_item = Menu_NONE;
	char choice2;
	int  sub_choice;
	int  op;
	put_value();
	new_board();


game:while(1)
	{
//		system("CLS");								// ����һ����ɾ�
		Welcome_Info( );							// ��ӡ��ӭ��Ϣ������ʼѡ��
		getchoice(choice, 10, "1r2n3l4s0e ��ʼѡ��Game Setting����\n1=����Rule,R��  2=���̣�New, N�� \n3=���̣�Load, L��  4=���̣�Save��S��  0=������End, E��");

		switch (choice) 
		{
			case '1' :
			case 'r' :
						menu_item = Menu1_Rule;
						//			system("CLS");
						sub_choice  = print_rule();
						if(sub_choice == 1)			// ������������Ϸ
							continue;				// else ���䵽������˳��ж�
			case '2' :								// ������ļ�ʧ�ܣ���Ҳ���뵽����ȥ�¿���
			case 'n' :
						menu_item = Menu1_New;
						new_board( );
						break;
			case '3' :
			case 'l' :
						menu_item = Menu1_Read;
						op = ReadBoard( );
						if ( Is_initialboard( ) ) {
							menu_item = Menu1_New;
						}
						else Current_Color( );
						break;
			case '4' :
			case 's' :
						op = SaveBoard( );
						if ( Is_initialboard( ) )
							continue;				// �����ԭʼ2 X 2���̣���������������ѡ��
						break;
						case '0' :
			case 'e' :
						menu_item = Menu1_End;
						printf("Do you really mean to leave the game? ���ǵ������˳���Ϸô��\n");
						printf("1.No\t���˳�\n0.Exit\t�˳�\n");
						getchoice(choice, 4, "1n0e �˳���Ϸ��Exit����:1=���ˣ�No, N��  0=�˳���Exit, E��");
						if(choice == '1' || choice == 'n')
							goto game;
						else return 0;
			default :
						break;
		}

		if ((menu_item == Menu1_Read) || (menu_item == Menu1_New))
		{
			char o_b;
//			system("CLS");
			level( );								// ѡ����

			if ( menu_item == Menu1_Read )
				DrawBoard( );

			if ( menu_item == Menu1_New ) 
			{
				printf("\nPlease choose your side(1/2):\n");
				printf("��ѡ����������(1/2)��\n");
				printf("1.Black��(go first)  ���ӣ����֣�\n2.White��(go second) ���ӣ����֣�\n");
			
				getchoice(o_b, 4, "1b2w ѡ����ɫ��Choose color����\n1=�������֣���Black��B��  2=���Ӻ��֣���White��W��");
				if (o_b == '1' || o_b == 'b')
				{
					HUMAN=BLACK;
					COMPUTER=WHITE;
					possible();
					printf("\nAs you chose the black side, you go first\n");
					printf("�������֣���������:\n");
				}
				else									// ����Ҫif(o_b == '2' || o_b == 'w')
				{
					HUMAN=WHITE;
					COMPUTER=BLACK;
					possible();
					DrawBoard(); 
					printf("\nAs you chose the white side, I go first\n");
					printf("���Ӻ��֣����ɸ���ǫ�ã����²������ˣ�\n");
					srand((unsigned)time(NULL));		// ��ʼ��
					int a=rand();						// �����
					int can[60][2];
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
			}

			start();

			printf("\nYour choice���������룩: \n1.New game\t����Ϸ\n2.Save  \t����\n0.Exit  \t�˻�\n");
			getchoice(choice2, 6, "1n2s0e ������Ϸ��Replay����1=����Ϸ��New��N��  2=���̣�Save��S��  0=�˳���Exit��E��");

			if ( choice2 == '1' ||  choice2 == 'n' )			// 1������Ϸ
			{
				new_board();
				continue;
			}
			else
			{
				if (choice2 == '2' ||  choice2 == 's' )			// �ȴ��̣������Ƿ��˳�
				{
					op = SaveBoard();
//					system("PAUSE");
				} 

				new_board();
				printf("\nAnd then ���»���ʲô�Ը���\n");
				printf("1.Replay ������Ϸ\n0.Exit   �����˳�\n");

				getchoice(choice, 4, "1r0e ��Ϸѡ��1=������Ϸ��Replay, R��  0=�˳���Ϸ(Exit, E)");
				if (choice == '0' || choice == 'e')
					break;
				else
					continue;
			}
		}
	}
}

void Current_Color( ) 
{
	printf("\nThe color of each side is,\n");
	printf("˫�������е��������࣬\n");

	printf("\tYou           ���µ�����   :");
	if (HUMAN == BLACK) 
		printf("��");
	else printf("��"); 
	printf("\n");

	printf("\tThe computer  ���µ�����   :");
	if (COMPUTER == BLACK) 
		printf("��");
	else printf("��"); 
	printf("\n");
}

void level( )										// ѡ������
{
	char o_b;
	printf("\nPlease input the game level (degree of difficulty):\n");
	printf("����������ѡ�����Ϸ�Ѷȣ�\n");
	printf("1.Simple    2.Normal    3.Hard\n");
	printf("  ��        �е�        ����\n");
	getchoice(o_b, 6, "1s2n3h ѡ���Ѷ�(Game Level)��\n1=�򵥣�Simple��S��  2=�еȣ�Normal,N��  3=���ѣ�Hard,H��");

	if(o_b == '1' ||  o_b == 's' )
		DEPTH = LOW;
	else if(o_b == '2' || o_b == 'n')
		DEPTH = MIDIUM;
	else // if(o_b == '3' || o_b == 'h')
		DEPTH = HIGH;
}
void start()
{
	int m_h,m_c;
	int steps=0;
	char now;
	int i,j;
	char ci,cj,c;									// ����
	char order_line[200];							// Ϊ�˱���һЩ��bug�Ŀ��������ַ�������
	int place_i;									// Ϊ�˳ɹ��з�ô��û�취����
	int lo;											// order's length
	int k;
	char choice;
	int  menu_item = Menu_NONE;
	int fileop;
	while(1)
	{
		possible();
		m_h=mobility(HUMAN);
		m_c=mobility(COMPUTER);
		if((m_h==0)&&(m_c)==0)
			break;
		if(steps%2==0)
		{
			printf("\nYour turn �ֵ�������\n");
			now=HUMAN;
		}
		else
		{
			printf("\nComputer's turn �ֵ�������\n");
			now=COMPUTER;
		}
		if(mobility(now)==0)
		{
			printf("\nImpossible for this side to put chess,move on\n");
			printf("��ǰѡ�����ӿ��£�����һλ����һ��\n");
			steps++;
			continue;
		}
		if(now==HUMAN)								// ��������
		{
			while(1)
			{
				gets_s(order_line);
				while(order_line[0]=='\0')
					gets_s(order_line);
				ci=order_line[0];
				place_i=0;
				lo=strlen(order_line);
				for(k=0;k<lo-2;k++)
				{
					if ((order_line[k]==' ')||(order_line[k]==',')||(order_line[k]=='/')||(order_line[k]=='.')||
						(order_line[k]=='(')||(order_line[k]==')')||(order_line[k]=='��')||(order_line[k]=='��'))
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
					if ((order_line[k]==' ')||(order_line[k]==',')||(order_line[k]=='/')||(order_line[k]=='.')||
						(order_line[k]=='(')||(order_line[k]==')')||(order_line[k]=='��')||(order_line[k]=='��'))
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
				if((ci>='8'))
				{
					printf("Use a correct number 0-7 for row , please ��ע���б귶Χ 0-7\n");
					continue;
				}
				if(((cj>'h')||(cj<'a'))&&((cj>'H')||(cj<'A')))
				{
					printf("Use a correct letter A-H for column , please ��ע���б귶Χ A-H\n");
					continue;
				}
				if((ci>='0')&&(ci<='7'))
					break;
				printf("Notice the form requested, please. A number for the row and a letter for the rol.\n");
				printf("No matter which is typed first and whether the letter is capitalized.\n");
				printf("��ע��������ʽ���������б꣨���֣����б꣨��ĸ�������д�Сд���Ⱥ�˳��\n");
			}
			if(ci=='-')
			{
				printf("\nWhat's the order,sir ��������ָ���ǣ�\n");
				printf("1.Help       ����\n");
				printf("2.Continue   ����\n");
				printf("3.Load       ����\n");
				printf("4.Save       ����\n");
				printf("0.Back       �˻�\n\n");

				getchoice(choice, 10, "1h2c3l4s0b ��Ϸѡ�Game Setting����\n1=������Help,H��  2=������Continue, C��  \n3=���̣�Load, L)   4=���̣�Save��S��  0=�˻أ�Back, B��");

				switch (choice) {
					case '1' :
					case 'h' :
								menu_item = Menu2_Help;
								printf("Your possible choices are:\n");
								printf("����������ǰ����ѡ�������֮��:\n");
								possible();
								for (i=0;i<SCALE;i++)
									for (j=0;j<SCALE;j++)
										if(human[i][j])		// ��������
											printf("\t%d %c\n",i,j+'A');
								system("PAUSE");				// 								printf("Press any key to continue\n");
								DrawBoard();
								continue;					// �������Ļ���ֱ���ֵ����������������Կ�����
					case '2' :
					case 'c' :
								menu_item = Menu2_Continue;
								DrawBoard();
								continue;

					case '3' :
					case 'l' :
								menu_item = Menu2_Read;
								fileop = ReadBoard();
								if ( fileop ) 
								{
									printf("\nFile loaded. �ļ��Ѷ���\n");
									level( );
								}
								else printf("Continue the game. ������Ϸ\n");
								Current_Color( );
								DrawBoard();
								continue;
					case '4' :
					case 's' :
								menu_item = Menu2_Save;
								if (SaveBoard())
									printf("\nFile saved. \n�ļ���д��\n");
								DrawBoard();
								// system("PAUSE");
								continue;
					case '0' :
					case 'b' :
								menu_item = Menu2_Back;
								return;
					default :
								break;

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
				printf("��Ǹ���������ڸô� (%d,%c) ���ӵ�\n",i,'A'+j);
			    continue;
		    }
			printf("Your choice is: %d %c\n",i,j+'A');
			printf("����ѡ���ǣ�    %d %c\n",i,j+'A');
			change_color(i,j,now,board);
//			system("CLS");
			DrawBoard();
			steps++;
		}
		else
		{
			find_position(0,board,-inf,inf);
			if(judge_put(comp_x,comp_y,COMPUTER,board)==0)  //debug�����Ǳ��մ�ʩ��ʡ����ѭ��
			{
				printf("%lld\n",find_position(0,board,-inf,inf));
				printf("%d\n",inf);
				printf("sSstem error\n");  
				printf("%d,%d is not available\n",comp_x,comp_y);
				printf("Please tell Patricia.Xiao in order to improve this program. Thanks a lot.\n");
				printf("ϵͳ���ֹ��ϣ��뼰ʱ֪ͨ��Ʒ��Ф֮��������Ľ�������ģ�лл���ķ�����\n\n");
				printf("Press any letter or number to move back\n");
				printf("����������ַ�����\n\n");
				char cv;
				scanf_s("%c",&cv);
//				return 0;
				return ;
			}
			change_color(comp_x,comp_y,now,board);
			//system("CLS");   // ���ﲻ�������ˣ��������ƣ��ۼҳ������׵ģ���ԩ���ó���
			DrawBoard();
			printf("\nThe last step is:%d %c\n",comp_x,comp_y+'A');
			printf("�����������ڣ�%d,%c�����ӵ�\n",comp_x,comp_y+'A');
			steps++;
		}
	}

	end_judge();											// �ж���ֵ�ʤ��
}

void getchoice(char &ch, int choices, char str[ ])
{
	char before[60],o_b;
	int l_b,i;

	while(1)
	{
		printf("Your choice ����������ѡ��\n");
		gets_s(before);
		while(before[0]=='\0')
			gets_s(before);

		l_b = strlen(before);
		o_b = before[0];
		for (i=0;i<l_b-1;i++)
		{
			if (before[i]==' ')
				o_b = before[i+1];
			else
			break;
		}
        if(isupper(o_b)) 
			o_b = tolower(o_b); 

		for (i = 0; i < choices; i++) 
			if (o_b == str[i]) {
				ch = o_b;
				return;
			}
		
		printf("\n��������ȷ��ѡ��: \n%s\n", &str[choices+1]);
	}
}

int SaveBoard()									// �����̱��浽�ļ���
{
//	system("CLS");
	int i, j, k = 0;
	FILE *fp = NULL;							// ��ʼ˭Ҳ��ָ��
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;

	while(fp==NULL)
	{
		printf("\nPlease input the name of the file in which the board to be saved\n");
		printf("( No more than 60 letters )");
		printf("�������ļ��� ( С��60�ַ� ) \n");
		gets_s(filename);
		
		while (_access(filename, 0) == 0)			// �Ѿ�����
		{
			printf("The file named '%s' has already existed, would you like to rewrite the file?\n", filename);
			printf( "�ļ�'%s'�Ѿ������ˣ���Ҫ�������𣿣�Y/N/Q��\n", filename);
			printf( "1.Yes\t\t����\n2.No\t\t�����ǻ����ļ�����\n0.Quit saving\t��������\n");
			getchoice(ch, 6, "1y2n0q �����ļ�(Cover the existed file)��\n1.��(Yes,Y)   2.��������Change: No, N��  0.�˳���Quit Saving, Q��");

			if(ch == '0' || ch == 'q' )
					return false;
			if (ch == '1' || ch == 'y')
				break;								// �Ǿ�����������ˣ������ء����˼Ҷ�˵�����ˣ�
			else 
				{
					printf("\nYour new choice of the file's nams:\n");
					printf("�����������ļ�����\n");
					gets_s(filename);
					continue;						// ��������Ҫ��ʲô���֡���
				}
		}

		err  = fopen_s( &fp, filename, "w+" );

		if( err != 0 )
		{
			printf("The effort we paid trying to open the file '%s' has failed, what returned is an error code %d",filename,err);
			printf( "�ļ�'%s'��ʧ�ܣ����ش����� %d \n", filename, err);

			printf("Another try ����ѡ���ļ�?\n");

			printf( "1.Yes\t����ѡ�ļ�\n0.No\t��������\n");
			getchoice(ch, 4, "1y0n ����ѡ�ļ�(Select File)��\n1.�ǣ�Yes,Y��  0.N=�˳���No Saving, N��");
			if(ch == '1' || ch == 'y' )
				continue;
			else
				break;								// �Ǿ�����������ˣ������ء����˼Ҷ�˵�����ˣ�
		}
	} 

	if (fp == NULL)									// ˵���Ƿ��������˳���
		return false;

	for(i=0;i<SCALE;i++)							// ����
	{
		for(j=0;j<SCALE;j++) 
		{
			str[k++] = board[i][j];
		}
		str[k++]='\n';								// �ӻس�
	}
	str[k++]=COMPUTER; 
	str[k++]=HUMAN;

    fwrite(str,1,(SCALE+1)*SCALE+2,fp);	// ����ԭ��size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
							// ע�⣬���� str��û�б���"\0"�ַ�����������������    fwrite(str,1,strlen(str),fp);	
    fclose(fp);  //�ǵù��ϡ���

	printf("File '%s' has been saved successfully \n",filename);
	printf("�ļ�'%s'�ɹ�д��\n", filename );
	return true;
}

//�ж��Ƿ�Ϸ����̵�ϵ�к���

//�ݹ�ר��
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;										// ���緵��
	if(!b[i][j])									// ��ֵ����
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);								// ��˷�����
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
	return;
}

int ReadBoard()										// ���ļ��ж�����������
{
	int i, j, k = 0;
	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // �Ǻǡ�����ô���ĳ��򶼱�����ˣ����һ��������������鳤��������ջ�������
	int  board_status = GoodBoard;
 read:
	ch='0';
	while(fp == NULL) 
	{
		printf("\nPlease input a file's name from which you'd like to get the board\n");
		printf("(warning: no more than 60 letters)\n");
		printf("�������ļ��� ( С�� 60 �ַ� ) \n");
		gets_s(filename);
		err  = fopen_s( &fp, filename, "r" );// fp = fopen(filename,"r+") ;
		if( err == 0 )
		{
			printf("File '%s' has been opened successfully\n", filename);
			printf( "�ļ�'%s'�ɹ���\n", filename );
			break;
		}
		else 
		{
			if (err == ENOENT)
			{
				printf("Sorry, the file '%s' does not exist\n",filename);
			    printf( "�ļ�'%s'������\n", filename);
			}
			else
			{
				printf("Returned an error code %d, the file you chose could not open.\n", filename, err);
			    printf( "�ļ�'%s'��ʧ�ܣ����ش����� %d \n", filename, err);
			}
			printf("Reinput a file's name ���������ļ�����(Y/N)\n");
			printf("1.Yes\t���¶��� \n0.No\t��������\n");
			getchoice(ch, 4, "1y0n ����ѡ��(Read a new file)��\n1=�����ļ���Yes, Y��  0=�������̣�No, N��");  
			if (ch == '0' || ch == 'n')
				return false;
		}
	}												//�ļ������Զ�Ӧ��ĳ�ĵ�

	fread(str,1,SCALE*(SCALE+1)+2,fp);	
	// ����ԭ��size_t fread(void *buffer, size_t size, size_t count, FILE *stream);��
		
	k = 0;
	for (i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++) 
		{
			save_board[i][j] = str[k++];
		}
		k++;										// ȥ���س�
	}
	char computer=str[k++];
	char human=str[k++];

    fclose(fp);  //�ù��͹غ�ϰ��

	board_status = board_check(save_board);

	switch (board_status) {
		case IllegalChar :
							printf("\nI'm sorry but there are some characters illegal in the file.\n");
							printf("�Բ�������ָ�����ļ��д��ڱ���Ϸ�޷�ʶ��ķ��ţ��޷�������\n");
							break;
		case NoCentralChess:
							printf("\nI'm sorry there are no original center chesses on the board.\nPlease use another one. Thank you!");
							printf("\n�Բ�����������û���������ӡ�������ѡ��лл������\n");
							break;
		case NotConnected :
							printf("\nI'm sorry that the board is not connect. Please use another one. Thank you!");
							printf("\n�Բ��������������Ӳ�������������ѡ��лл������\n");
							break;
		case GoodBoard :
							for(i=0;i<SCALE;i++)
								for(j=0;j<SCALE;j++)
									board[i][j] = save_board[i][j];

							HUMAN = human;
							COMPUTER = computer;
							return true;
		default :
							break;
	}
	 
	printf("\nSelect another file to open, or give up?\n");
	printf("��������ѡ��Ŀ���ļ����̣����Ƿ������̣�\n");
	printf("1.Yes\t���¶��� \n0.No\t��������\n");
	getchoice(ch, 4, "1y0n ����ѡ��(Read a new file)��\n1=�����ļ���Yes, Y��  0=�������̣�No, N��");  
	if ( ch == '1' || ch == 'y')
	{
		fp=NULL;
		goto read;
	 }
	else return false;
	return true;									// ��ʵִ�в������ǰ��ɹ��Ѿ�����
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
	printf("\n    A negative integer for help. ����һ�����������ɻ�ð���\n\n");

	// ��ӡ��һ�еı�� A B C D E F G H
	printf("\t\t      A");							// ��һ��Ҫ������ӡ
	for(i=1;i<n;i++) {								// ��ӡ�����ĺ�������Ϣ
		printf("%4c",char(i+int('A')));
	}
	printf("\n");

	// ��ӡ��һ���������
	printf("\t\t   ����");							// ���Ͻǵ�һ��Ҫ������ӡ
	for(i=1;i<n;i++) {								// ��ӡ�����ĸ���
		printf("�С�");
	}
	printf("��\n");									// ��ӡ���Ͻ� 


	// ��ӡ�ӵ�0�е���n-2�е����̣�ÿһ�������Լ��ָ���
    for(i=0;i<n-1;i++)
    { 
		// �����ӡÿһ�У�ÿ���������Ҷ��������ꣻÿ�ж��ȴ����ݣ��ٴ�һ�д��ĺ��߷ָ�
		printf("\t\t%2d ", i);						// �ȴ��i�б�ǣ������꣩
	
		// ��ӡ��j�е�ʵ�����ݣ����ӵ�0����n-2�е������Լ��ָ���
		for(j=0;j<n;j++) {
			printf("��");							// ��ӡ�ָ����
			//DrawChess(i,j,board);					// ��ӡ������̷���
			DrawChess(i,j);
		}
		printf("��%2d\n",i);						// ��ӡ���һ�е�ǽ���Լ�����

		// ��һ�д��ĺ��߷ָ�
		printf("\t\t   ��");						// ��ӡ�м�ָ�ǽ����߽�
		for(j=0;j<n-1;j++) {						// ��ӡ�ָ�ǽ
			 printf("����");
		}
         printf("����\n");							// ��ӡ�м�ָ�ǽ���ұ߽�
    }

	// ��ӡ������һ��
	printf("\t\t%2d ", n-1);							// �ȴ�ӡ���һ�б�ǣ������꣩
	// ��ӡ��j��
	for(j=0;j<n;j++) {
			printf("��");							// ��ӡ�ָ����
			DrawChess(n-1,j);
	}
	printf("��%2d\n",n-1);							// ��ӡ���һ�е�ǽ���Լ�����
	
	
	// ��ӡ���һ�����̵���ײ�
	printf("\t\t   ����");							// ���½ǵ�һ��Ҫ������ӡ
	for(i=0;i<n-1;i++) {							// ��ӡ����������һ�и���
		printf("�ء�");
	}
	printf("��\n");									// ��ӡ���Ͻ�


	// ��ӡ���һ�е����� A B C D E F G H
	printf("\t\t      A");							// ��һ��Ҫ������ӡ
	for(i=1;i<n;i++) {								// ��ӡ�����ĺ�������Ϣ
		printf("%4c",char(i+int('A')));
	}
	printf("\n");	
}

// ��ӡi,jλ��ͼ��
void DrawChess(int i, int j)
{
	switch(board[i][j]) {
		case EMPTY:		printf("  ");				// �ո���
						break;
		case BLACK:		printf("��");				// ����
						break;
		case WHITE:	    printf("��");				// ����
						break;
		default:		break;
	}
	return;
}

void Welcome_Info( )
{
	printf("\n");
	printf("�X�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�[\n");
	printf("�U                                                            �U\n");
	printf("�U                                           NO.1200012850    �U\n");
	printf("�U                                                            �U\n"); 
	printf("�U     OTHELLO GAME PROGRAMED BY PATRICIA.XIAO                �U\n"); 
	printf("�U     �ڰ����˻���սС��Ϸ Ф֮����Ʒ                        �U\n"); 
	printf("�U                                                            �U\n"); 
	printf("�U     Welcome to play Othello with me~                       �U\n"); 
	printf("�U     Enjoy yourself and have a great time~~~                �U\n"); 
	printf("�U     ��ӭʹ�ü��װ�ڰ����սС���� ף�������� ~~~        �U\n"); 
	printf("�U                                                            �U\n");
	printf("�U            1.RULES        ����˵��                         �U\n"); 
	printf("�U            2.NEW START    �¿�ʼ                           �U\n"); 
	printf("�U            3.LOAD         ����                             �U\n"); 
	printf("�U            4.SAVE         ����                             �U\n"); 
	printf("�U            0.END & EXIT   ����                             �U\n");
	printf("�U                                                            �U\n");
	printf("�^�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�T�a\n"); 
	printf("\tSelet a choice from the above, please:\n");
	printf("\t������0-4�е�ĳ�����֣���˵�����ĸ��\n\n"); 
}

int print_rule()
{
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
		printf("\tAfter reading the introduction, what's your decision next?\n");
		printf("\t�����˵Ļ���ѡ�������Ϸ�������˳���Ϸ��\n");
		printf("\t1.Continue ������Ϸ\n \t0.Leave    �˳���Ϸ\n");

		char ch;
		getchoice( ch, 4, "1c0l ����ѡ��Selection��:\n1=������Continue, C��  0=�˳���Leave��L��" );
		if (ch == '1' || ch == 'c') 
			return 1;
		else return 2;
}


void end_judge()
{
	int h=0,c=0;
	int i,j;

	for (i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==HUMAN)
				h++;
			else
				if(board[i][j]==COMPUTER)
					c++;
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
	printf("\n\n  ��Ϸ����~~~\n  ���ڹ������������\n\n");
	printf("  ���ս����\n\n\t  ������ʣ��%d�������������ʣ��%d����\n\n",h,c);
	if(h<c)
	    printf("  ���ź������ˣ�����������٣����ֿ��Ǽ���������Ѿ�������������\n");
	else
		if(h>c)
			printf("  ���ģ�����������²�Ӯ����������Ҫ���찡����������ˣ�\n");
		else
		    printf("  ƽ�ְ�~~~�ѷ�ʤ����˵~~~��Ȼ����һ�̰�~~~\n");
    printf("                                                                �U\n");
	printf("                                                            �T�T�a\n");
}


//�ж��Ƿ�Ϸ����̵�ϵ�к���
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
				temp[i][j]=1;
			else
				if(a[i][j]==EMPTY)
				    temp[i][j]=0;
				else								// ���ڷǷ��ַ�
					return IllegalChar;
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
			if(!temp[i][j])							// �������û�����ӣ�һ���Ƿ�
				return NoCentralChess;
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
			if(temp[i][j])
				return NotConnected;				// δ����һƬ��һ������
	}
	return GoodBoard;
}

void put_value()
{
	int i,j;
	for(i=0;i<SCALE;i++)
		for(j=0;j<SCALE;j++)
			const_value[i][j]=1;
	const_value[0][0]=const_value[0][SCALE-1]
	=const_value[SCALE-1][0]=const_value[SCALE-1][SCALE-1]=5000;
	//origin:800
	int edge = 300;
	//origin:100
	for(i=1;i<SCALE-1;i++)
	{
		const_value[0][i]=edge;
		const_value[SCALE-1][i]=edge;
		const_value[i][0]=edge;
		const_value[i][SCALE-1]=edge;
	}
	int corner = -1000;
	//origin:-200
	const_value[0][1]=const_value[1][0]=const_value[1][1]=corner;
	const_value[0][SCALE-2]=const_value[1][SCALE-2]=const_value[1][SCALE-1]=corner;
	const_value[SCALE-1][1]=const_value[SCALE-2][0]=const_value[SCALE-2][1]=corner;
	const_value[SCALE-1][SCALE-2]=const_value[SCALE-2][SCALE-1]=const_value[SCALE-2][SCALE-2]=corner;

	const_value[0][3]=const_value[0][4]=-100;
	const_value[SCALE-1][3]=const_value[SCALE-1][4]=-100;
	const_value[3][0]=const_value[4][0]=-10;
	const_value[3][SCALE-1]=const_value[4][SCALE-1]=-100;
}

//�ж��ܷ����ӵĺ���
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)								// �����ص�����
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)								// �����ص�����
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
		for(j=0;j<SCALE;j++)
		{
			human[i][j]=computer[i][j]=0;
			if(judge_put(i,j,HUMAN,board))
				human[i][j]=1;
			if(judge_put(i,j,COMPUTER,board))
				computer[i][j]=1;
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
	else  // bug= =bû�Ӵ����ţ��Ҿ�������һ�£������𡭡���
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
	// ��������
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


//��ֵ����
//����ǰ��ֹ�ֵ
//Ѱ�����ŵ������ϵ�к���

void c_val(char board[SCALE][SCALE])   //����ɱ�Ȩ��
{
	int i,j;
	int n=SCALE-1;
	for(i=0;i<=n;i++)
		for(j=0;j<=n;j++)
			change_value[i][j]=0;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// ��
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	// �Է�ռ�˽ǵ���������Դ���
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1] =- 300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1] =- 300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1] =- 300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2] =- 300;
	// �Է�ռ�ߵĲ��Ա仯�ִ���
	for(i=1;i<SCALE-1;i++)
	{
		if(board[0][i]==HUMAN)
		{
			if((i != 1)&&(change_value[0][i-2] != HUMAN))
				change_value[0][i-1]-=300;
			if((i != SCALE-2)&&(change_value[0][i+2] != HUMAN))
				change_value[0][i+1]-=300;
		}
		if(board[SCALE-1][i]==HUMAN)
		{
			if((i != 1)&&(change_value[SCALE-1][i-2] != HUMAN))
				change_value[SCALE-1][i-1]-=300;
			if((i != SCALE-2)&&(change_value[SCALE-1][i+2] != HUMAN))
				change_value[SCALE-1][i+1]-=300;
		}
		if(board[i][0]==HUMAN)
		{
			if((i != 1)&&(change_value[i-2][0] != HUMAN))
				change_value[i-1][0]=-300;
			if((i != SCALE-2)&&(change_value[i+2][0] != HUMAN))
				change_value[i+1][0]=-300;
		}
		if(board[i][SCALE-1]==HUMAN)
		{
			if((i != 1)&&(change_value[i-2][SCALE-1] != HUMAN))
				change_value[i-1][SCALE-1]-=300;
			if((i != SCALE-2)&&(change_value[i+2][SCALE-1] != HUMAN))
				change_value[i+1][SCALE-1]-=300;
		}
	}
	//����ռ�ǵ�������Դ���
	int fight = 1000;
	//origin:300
	//���Ͻ�
	if(board[0][1]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][0]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][1]!=EMPTY)
		change_value[0][0]+=fight;
	//���Ͻ�
	if(board[0][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-1]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	//���½�
	if(board[SCALE-1][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][0]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	//���½�
	if(board[SCALE-2][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-1][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-2][SCALE-1]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
}


//��ֵ����
//����ǰ��ֹ�ֵ

int temp_value(char board[SCALE][SCALE])		// �Ե��и߼��в�ͬ�ļ���Ȩ�ط���
{
	for(int i = 0 ; i < SCALE ; i++)
		for(int j = 0 ; j < SCALE ; j++)
			change_value[i][j] = 0;
	if ( DEPTH == HIGH )						// �������еͼ��Ͳ����㶯̬Ȩ�أ�ֻ����߼���
		c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				if(DEPTH != LOW)		// ���ͼ��������ڼ�����µ�ʱ�򲻼����ȶ��ԡ�����ҿ���
					score+=steady(i,j,COMPUTER,board)*30;	// �ȶ���
				score+=(const_value[i][j]+change_value[i][j]);      //λ�ù���Ȩ��+�ɱ�Ȩ��
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //ע��change_valueȫ���Լ�����Ƕ�Ϊ׼�ģ�
				}
			if(DEPTH != LOW)
			{
				if(judge_put(i,j,COMPUTER,board))			// �ж���
					score+=100;
				if(judge_put(i,j,HUMAN,board))
					score-=100;
			}
			else
			{
				int num_c = 1000; 
				int num_h = 1000;
				//origin:400//origin:500//origin:600
				if(judge_put(i,j,COMPUTER,board))			// �ж���
					score+=num_c;
					//score += (const_value[i][j] + change_value[i][j]);
				if(judge_put(i,j,HUMAN,board))
					score-=num_h;
					//score -= (const_value[i][j] + change_value[i][j]);
					//score -= const_value[i][j];
			}
		}
	}
	return score;
}

// ��alpha-beta��֦
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;										// ����
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
			if(judge_put(i,j,now,last_board))			// ��������֮��
			{
				change_color(i,j,now,temp_board);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++) 
					{
						temp_board[p][q]=last_board[p][q];	// ��ԭ
					}
				if(depth%2==0)							// �û�����
				{
					if(a == 0)							// ֻ��Ϊ��ȷ���н�
														// ��Ȼ����Ҳ�����˵�ǲ���ȫ�Ħ��¼�֦
														// ��Ϊ�ҵ�min - max�ṹ����Ҳ���Ǻ���ͳ��
				    {
					    alpha = temp;						// �ð���û˵�滰���滰���ҵ����������ÿ����ܳ������ͷ�Χ�ġ�
						temp_x = i;
					    temp_y = j;
						comp_x = i;
						comp_y = j;

					    a++;
					    continue;  
				    }
				    if(alpha < temp)
				    {
					    alpha = temp;
						temp_x = i;
						temp_y = j;
				    }
					if(alpha >= beta)
					{
						if(depth == 0)
						{
							comp_x = temp_x;
							comp_y = temp_y;				// �����������Ǵ������ﰡ��������ָ���˲��𡭡�
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)						// �˵�һ��
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