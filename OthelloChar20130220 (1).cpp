
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
#define SCALE		 8								//黑白棋规模一般是 8 X 8

#define LOW			1								// 低级棋力，贪心法
#define MIDIUM		3								// 中级棋力，搜三层
#define HIGH		5								// 高级棋力，搜五层

#define Menu_NONE		10							// 无意义选项
#define Menu1_Rule		1							// 求助
#define Menu1_New		2							// 写盘
#define Menu1_Read		3							// 读盘
#define Menu1_Save		4							// 写盘
#define Menu1_End		0							// 退出

#define Menu2_Help		1							// 求助
#define Menu2_Continue	2							// 写盘
#define Menu2_Read		3							// 读盘
#define Menu2_Save		4							// 写盘
#define Menu2_Back		0							// 退回

#define Menu_Level		7							// 棋力

#define FileNameLEN  60

#define inf          0xfffffff						// 最后似乎没派上什么用场？

char HUMAN = BLACK;									// 纯粹为了存空棋盘的话别出bug
char COMPUTER = WHITE;
int  DEPTH = HIGH;									// 搜索深度

char board[SCALE][SCALE];
char save_board[SCALE][SCALE];						// 读盘之前放进来处理用的

#define  IllegalChar		1						// 文件中有非法字符 
#define  NoCentralChess		2						// 中间没有棋子
#define  NotConnected		3						// 未连成一片
#define  GoodBoard			0						// 正常棋盘

int  human[SCALE][SCALE]={{0}};
int  computer[SCALE][SCALE]={{0}};					// 能否落子

int  comp_x,comp_y;									// 最新落子点

void change_color(int , int , char, char [SCALE][SCALE]);
int  judge_put(int ,int , char, char [SCALE][SCALE]);
void possible();
int  mobility(char);
void end_judge();

//每个位置的权重
int const_value[SCALE][SCALE];						// 固有部分
int change_value[SCALE][SCALE];						// 时时在变

void DrawBoard();
int  print_rule();

void Level( );										// 选择搜索深度

int  judge_put(int ,int , char, char [SCALE][SCALE]);
void possible( );
int  mobility(char);
long long find_position(int, char[SCALE][SCALE], long long, long long);

void put_value( );									// 给棋盘格赋权重

void new_board();									// 设置 2 x 2 对阵的初始棋局
int board_check(char [SCALE][SCALE]);				// 检查读入的棋盘的合法性
int  ReadBoard( );									// 读盘中棋局
int  SaveBoard( );									// 将棋局存盘
void getchoice(char &, int, char []);				// 读取选择值
void DrawBoard( );									// 画棋盘中的黑白棋子
void Current_Color( );								// 当前计算机与人的执棋颜色
void PrintScore( );									// 打印玩家和计算机的得分
void choose_Color( );								// 选择棋子颜色，黑色先走（一般都选它）
void start( );										// 程序的主要控制函数, void 也可以
void level( );										// 选择棋力
void Welcome_Info( );

// 判断棋盘是否初始棋盘
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
//		system("CLS");								// 把上一局清干净
		Welcome_Info( );							// 打印欢迎信息，并开始选择
		getchoice(choice, 10, "1r2n3l4s0e 开始选择（Game Setting）：\n1=规则（Rule,R）  2=新盘（New, N） \n3=读盘（Load, L）  4=存盘（Save，S）  0=结束（End, E）");

		switch (choice) 
		{
			case '1' :
			case 'r' :
						menu_item = Menu1_Rule;
						//			system("CLS");
						sub_choice  = print_rule();
						if(sub_choice == 1)			// 读完规则继续游戏
							continue;				// else 则落到下面的退出判断
			case '2' :								// 如果读文件失败，那也落入到这里去新开局
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
							continue;				// 如果是原始2 X 2棋盘，则继续让玩家重新选择
						break;
						case '0' :
			case 'e' :
						menu_item = Menu1_End;
						printf("Do you really mean to leave the game? 您是当真想退出游戏么？\n");
						printf("1.No\t不退出\n0.Exit\t退出\n");
						getchoice(choice, 4, "1n0e 退出游戏（Exit？）:1=不退（No, N）  0=退出（Exit, E）");
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
			level( );								// 选棋力

			if ( menu_item == Menu1_Read )
				DrawBoard( );

			if ( menu_item == Menu1_New ) 
			{
				printf("\nPlease choose your side(1/2):\n");
				printf("请选择您的棋子(1/2)：\n");
				printf("1.Black●(go first)  黑子（先手）\n2.White◎(go second) 白子（后手）\n");
			
				getchoice(o_b, 4, "1b2w 选择棋色（Choose color）：\n1=黑子先手（●Black，B）  2=白子后手（◎White，W）");
				if (o_b == '1' || o_b == 'b')
				{
					HUMAN=BLACK;
					COMPUTER=WHITE;
					possible();
					printf("\nAs you chose the black side, you go first\n");
					printf("黑子先手，阁下先请:\n");
				}
				else									// 不需要if(o_b == '2' || o_b == 'w')
				{
					HUMAN=WHITE;
					COMPUTER=BLACK;
					possible();
					DrawBoard(); 
					printf("\nAs you chose the white side, I go first\n");
					printf("白子后手，承蒙阁下谦让，在下不客气了：\n");
					srand((unsigned)time(NULL));		// 初始化
					int a=rand();						// 随机数
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

			printf("\nYour choice（现在您想）: \n1.New game\t新游戏\n2.Save  \t存盘\n0.Exit  \t退回\n");
			getchoice(choice2, 6, "1n2s0e 继续游戏（Replay）：1=新游戏（New，N）  2=存盘（Save，S）  0=退出（Exit，E）");

			if ( choice2 == '1' ||  choice2 == 'n' )			// 1是新游戏
			{
				new_board();
				continue;
			}
			else
			{
				if (choice2 == '2' ||  choice2 == 's' )			// 先存盘，再问是否退出
				{
					op = SaveBoard();
//					system("PAUSE");
				} 

				new_board();
				printf("\nAnd then 阁下还有什么吩咐？\n");
				printf("1.Replay 继续游戏\n0.Exit   彻底退出\n");

				getchoice(choice, 4, "1r0e 游戏选择：1=继续游戏（Replay, R）  0=退出游戏(Exit, E)");
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
	printf("双方所持有的棋子种类，\n");

	printf("\tYou           阁下的棋子   :");
	if (HUMAN == BLACK) 
		printf("●");
	else printf("◎"); 
	printf("\n");

	printf("\tThe computer  在下的棋子   :");
	if (COMPUTER == BLACK) 
		printf("●");
	else printf("◎"); 
	printf("\n");
}

void level( )										// 选择棋力
{
	char o_b;
	printf("\nPlease input the game level (degree of difficulty):\n");
	printf("请输入您所选择的游戏难度：\n");
	printf("1.Simple    2.Normal    3.Hard\n");
	printf("  简单        中等        较难\n");
	getchoice(o_b, 6, "1s2n3h 选择难度(Game Level)：\n1=简单（Simple，S）  2=中等（Normal,N）  3=较难（Hard,H）");

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
	char ci,cj,c;									// 落子
	char order_line[200];							// 为了避免一些出bug的可能性用字符串读入
	int place_i;									// 为了成功切分么，没办法的事
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
			printf("\nYour turn 轮到阁下了\n");
			now=HUMAN;
		}
		else
		{
			printf("\nComputer's turn 轮到在下了\n");
			now=COMPUTER;
		}
		if(mobility(now)==0)
		{
			printf("\nImpossible for this side to put chess,move on\n");
			printf("当前选手无子可下，请下一位多走一步\n");
			steps++;
			continue;
		}
		if(now==HUMAN)								// 该人下了
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
						(order_line[k]=='(')||(order_line[k]==')')||(order_line[k]=='（')||(order_line[k]=='）'))
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
						(order_line[k]=='(')||(order_line[k]==')')||(order_line[k]=='（')||(order_line[k]=='）'))
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
					printf("Use a correct number 0-7 for row , please 请注意行标范围 0-7\n");
					continue;
				}
				if(((cj>'h')||(cj<'a'))&&((cj>'H')||(cj<'A')))
				{
					printf("Use a correct letter A-H for column , please 请注意列标范围 A-H\n");
					continue;
				}
				if((ci>='0')&&(ci<='7'))
					break;
				printf("Notice the form requested, please. A number for the row and a letter for the rol.\n");
				printf("No matter which is typed first and whether the letter is capitalized.\n");
				printf("请注意输入形式，请输入行标（数字），列标（字母），不拘大小写与先后顺序\n");
			}
			if(ci=='-')
			{
				printf("\nWhat's the order,sir 请问您的指令是？\n");
				printf("1.Help       求助\n");
				printf("2.Continue   继续\n");
				printf("3.Load       读盘\n");
				printf("4.Save       存盘\n");
				printf("0.Back       退回\n\n");

				getchoice(choice, 10, "1h2c3l4s0b 游戏选项（Game Setting）：\n1=求助（Help,H）  2=继续（Continue, C）  \n3=读盘（Load, L)   4=存盘（Save，S）  0=退回（Back, B）");

				switch (choice) {
					case '1' :
					case 'h' :
								menu_item = Menu2_Help;
								printf("Your possible choices are:\n");
								printf("以下是您当前可以选择的落子之处:\n");
								possible();
								for (i=0;i<SCALE;i++)
									for (j=0;j<SCALE;j++)
										if(human[i][j])		// 人能落子
											printf("\t%d %c\n",i,j+'A');
								system("PAUSE");				// 								printf("Press any key to continue\n");
								DrawBoard();
								continue;					// 不这样的话就直接轮到计算机了让我情何以堪……
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
									printf("\nFile loaded. 文件已读入\n");
									level( );
								}
								else printf("Continue the game. 继续游戏\n");
								Current_Color( );
								DrawBoard();
								continue;
					case '4' :
					case 's' :
								menu_item = Menu2_Save;
								if (SaveBoard())
									printf("\nFile saved. \n文件已写出\n");
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
				printf("抱歉，您不能在该处 (%d,%c) 落子的\n",i,'A'+j);
			    continue;
		    }
			printf("Your choice is: %d %c\n",i,j+'A');
			printf("您的选择是：    %d %c\n",i,j+'A');
			change_color(i,j,now,board);
//			system("CLS");
			DrawBoard();
			steps++;
		}
		else
		{
			find_position(0,board,-inf,inf);
			if(judge_put(comp_x,comp_y,COMPUTER,board)==0)  //debug，这是保险措施，省得死循环
			{
				printf("%lld\n",find_position(0,board,-inf,inf));
				printf("%d\n",inf);
				printf("sSstem error\n");  
				printf("%d,%d is not available\n",comp_x,comp_y);
				printf("Please tell Patricia.Xiao in order to improve this program. Thanks a lot.\n");
				printf("系统出现故障，请及时通知出品人肖之屏，她会改进本程序的，谢谢您的反馈。\n\n");
				printf("Press any letter or number to move back\n");
				printf("请键入任意字符返回\n\n");
				char cv;
				scanf_s("%c",&cv);
//				return 0;
				return ;
			}
			change_color(comp_x,comp_y,now,board);
			//system("CLS");   // 这里不加清屏了，让你瞧瞧，咱家程序不作弊的，别冤枉好程序
			DrawBoard();
			printf("\nThe last step is:%d %c\n",comp_x,comp_y+'A');
			printf("方才在下是在（%d,%c）落子的\n",comp_x,comp_y+'A');
			steps++;
		}
	}

	end_judge();											// 判断棋局的胜负
}

void getchoice(char &ch, int choices, char str[ ])
{
	char before[60],o_b;
	int l_b,i;

	while(1)
	{
		printf("Your choice 请输入您的选择：\n");
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
		
		printf("\n请输入正确的选择: \n%s\n", &str[choices+1]);
	}
}

int SaveBoard()									// 把棋盘保存到文件中
{
//	system("CLS");
	int i, j, k = 0;
	FILE *fp = NULL;							// 初始谁也不指向
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;

	while(fp==NULL)
	{
		printf("\nPlease input the name of the file in which the board to be saved\n");
		printf("( No more than 60 letters )");
		printf("请输入文件名 ( 小于60字符 ) \n");
		gets_s(filename);
		
		while (_access(filename, 0) == 0)			// 已经存在
		{
			printf("The file named '%s' has already existed, would you like to rewrite the file?\n", filename);
			printf( "文件'%s'已经存在了，需要覆盖它吗？（Y/N/Q）\n", filename);
			printf( "1.Yes\t\t覆盖\n2.No\t\t不覆盖换个文件名称\n0.Quit saving\t放弃存盘\n");
			getchoice(ch, 6, "1y2n0q 覆盖文件(Cover the existed file)：\n1.是(Yes,Y)   2.换个名（Change: No, N）  0.退出（Quit Saving, Q）");

			if(ch == '0' || ch == 'q' )
					return false;
			if (ch == '1' || ch == 'y')
				break;								// 那就用这个名字了（管他呢……人家都说可以了）
			else 
				{
					printf("\nYour new choice of the file's nams:\n");
					printf("请重新输入文件名：\n");
					gets_s(filename);
					continue;						// 继续问他要用什么名字……
				}
		}

		err  = fopen_s( &fp, filename, "w+" );

		if( err != 0 )
		{
			printf("The effort we paid trying to open the file '%s' has failed, what returned is an error code %d",filename,err);
			printf( "文件'%s'打开失败，返回错误码 %d \n", filename, err);

			printf("Another try 重新选择文件?\n");

			printf( "1.Yes\t重新选文件\n0.No\t放弃存盘\n");
			getchoice(ch, 4, "1y0n 重新选文件(Select File)：\n1.是（Yes,Y）  0.N=退出（No Saving, N）");
			if(ch == '1' || ch == 'y' )
				continue;
			else
				break;								// 那就用这个名字了（管他呢……人家都说可以了）
		}
	} 

	if (fp == NULL)									// 说明是放弃存盘退出的
		return false;

	for(i=0;i<SCALE;i++)							// 存盘
	{
		for(j=0;j<SCALE;j++) 
		{
			str[k++] = board[i][j];
		}
		str[k++]='\n';								// 加回车
	}
	str[k++]=COMPUTER; 
	str[k++]=HUMAN;

    fwrite(str,1,(SCALE+1)*SCALE+2,fp);	// 函数原型size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream);
							// 注意，这里 str并没有保存"\0"字符串结束符，不能用    fwrite(str,1,strlen(str),fp);	
    fclose(fp);  //记得关上……

	printf("File '%s' has been saved successfully \n",filename);
	printf("文件'%s'成功写入\n", filename );
	return true;
}

//判断是否合法棋盘的系列函数

//递归专用
void walk(int i, int j, int b[SCALE][SCALE])
{
	if((i<0)||(i>=SCALE)||(j<0)||(j>=SCALE))
		return;										// 出界返回
	if(!b[i][j])									// 空值返回
		return;
	b[i][j]=0;
	walk(i-1,j-1,b);								// 向八方蔓延
	walk(i-1,j,b);
	walk(i-1,j+1,b);
	walk(i,j-1,b);
	walk(i,j+1,b);
	walk(i+1,j-1,b);
	walk(i+1,j,b);
	walk(i+1,j+1,b);
	return;
}

int ReadBoard()										// 从文件中读入棋盘数据
{
	int i, j, k = 0;
	FILE *fp = NULL;
	errno_t err;
	char filename[FileNameLEN];
	char str[(SCALE+1)*SCALE+2], ch;  // 呵呵……这么长的程序都编过来了，最后一个错是设错了数组长度以至于栈溢出……
	int  board_status = GoodBoard;
 read:
	ch='0';
	while(fp == NULL) 
	{
		printf("\nPlease input a file's name from which you'd like to get the board\n");
		printf("(warning: no more than 60 letters)\n");
		printf("请输入文件名 ( 小于 60 字符 ) \n");
		gets_s(filename);
		err  = fopen_s( &fp, filename, "r" );// fp = fopen(filename,"r+") ;
		if( err == 0 )
		{
			printf("File '%s' has been opened successfully\n", filename);
			printf( "文件'%s'成功打开\n", filename );
			break;
		}
		else 
		{
			if (err == ENOENT)
			{
				printf("Sorry, the file '%s' does not exist\n",filename);
			    printf( "文件'%s'不存在\n", filename);
			}
			else
			{
				printf("Returned an error code %d, the file you chose could not open.\n", filename, err);
			    printf( "文件'%s'打开失败，返回错误码 %d \n", filename, err);
			}
			printf("Reinput a file's name 重新输入文件名？(Y/N)\n");
			printf("1.Yes\t重新读盘 \n0.No\t放弃读盘\n");
			getchoice(ch, 4, "1y0n 读盘选择(Read a new file)：\n1=读新文件（Yes, Y）  0=放弃读盘（No, N）");  
			if (ch == '0' || ch == 'n')
				return false;
		}
	}												//文件名可以对应到某文档

	fread(str,1,SCALE*(SCALE+1)+2,fp);	
	// 函数原型size_t fread(void *buffer, size_t size, size_t count, FILE *stream);　
		
	k = 0;
	for (i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++) 
		{
			save_board[i][j] = str[k++];
		}
		k++;										// 去掉回车
	}
	char computer=str[k++];
	char human=str[k++];

    fclose(fp);  //用过就关好习惯

	board_status = board_check(save_board);

	switch (board_status) {
		case IllegalChar :
							printf("\nI'm sorry but there are some characters illegal in the file.\n");
							printf("对不起，您所指定的文件中存在本游戏无法识别的符号，无法正常打开\n");
							break;
		case NoCentralChess:
							printf("\nI'm sorry there are no original center chesses on the board.\nPlease use another one. Thank you!");
							printf("\n对不起，您棋盘中没有中心棋子。请重新选择，谢谢合作！\n");
							break;
		case NotConnected :
							printf("\nI'm sorry that the board is not connect. Please use another one. Thank you!");
							printf("\n对不起，您棋盘中棋子不连续。请重新选择，谢谢合作！\n");
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
	printf("您想重新选择目标文件读盘，还是放弃读盘？\n");
	printf("1.Yes\t重新读盘 \n0.No\t放弃读盘\n");
	getchoice(ch, 4, "1y0n 读盘选择(Read a new file)：\n1=读新文件（Yes, Y）  0=放弃读盘（No, N）");  
	if ( ch == '1' || ch == 'y')
	{
		fp=NULL;
		goto read;
	 }
	else return false;
	return true;									// 其实执行不到这里，前面成功已经返回
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
	printf("\n    A negative integer for help. 输入一个负整数即可获得帮助\n\n");

	// 打印第一行的标记 A B C D E F G H
	printf("\t\t      A");							// 第一个要单独打印
	for(i=1;i<n;i++) {								// 打印其他的横坐标信息
		printf("%4c",char(i+int('A')));
	}
	printf("\n");

	// 打印第一行棋盘最顶层
	printf("\t\t   ┌—");							// 左上角第一个要单独打印
	for(i=1;i<n;i++) {								// 打印其他的格线
		printf("┬—");
	}
	printf("┐\n");									// 打印右上角 


	// 打印从第0行到第n-2行的棋盘，每一行数据以及分割线
    for(i=0;i<n-1;i++)
    { 
		// 下面打印每一行，每行最左最右都有行坐标；每行都先打数据，再打一行纯的横线分割
		printf("\t\t%2d ", i);						// 先打的i行标记（纵坐标）
	
		// 打印第j列的实质内容，即从第0到第n-2列的数据以及分割线
		for(j=0;j<n;j++) {
			printf("│");							// 打印分割符号
			//DrawChess(i,j,board);					// 打印这个棋盘符号
			DrawChess(i,j);
		}
		printf("│%2d\n",i);						// 打印最后一列的墙，以及坐标

		// 打一行纯的横线分割
		printf("\t\t   ├");						// 打印中间分割墙的左边界
		for(j=0;j<n-1;j++) {						// 打印分割墙
			 printf("—┼");
		}
         printf("—┤\n");							// 打印中间分割墙的右边界
    }

	// 打印最后的那一行
	printf("\t\t%2d ", n-1);							// 先打印最后一行标记（纵坐标）
	// 打印第j列
	for(j=0;j<n;j++) {
			printf("│");							// 打印分割符号
			DrawChess(n-1,j);
	}
	printf("│%2d\n",n-1);							// 打印最后一列的墙，以及坐标
	
	
	// 打印最后一行棋盘的最底层
	printf("\t\t   └—");							// 左下角第一个要单独打印
	for(i=0;i<n-1;i++) {							// 打印其他的最下一行格线
		printf("┴—");
	}
	printf("┘\n");									// 打印右上角


	// 打印最后一行的座标 A B C D E F G H
	printf("\t\t      A");							// 第一个要单独打印
	for(i=1;i<n;i++) {								// 打印其他的横坐标信息
		printf("%4c",char(i+int('A')));
	}
	printf("\n");	
}

// 打印i,j位的图像
void DrawChess(int i, int j)
{
	switch(board[i][j]) {
		case EMPTY:		printf("  ");				// 空格子
						break;
		case BLACK:		printf("●");				// 黑棋
						break;
		case WHITE:	    printf("◎");				// 白棋
						break;
		default:		break;
	}
	return;
}

void Welcome_Info( )
{
	printf("\n");
	printf("╔══════════════════════════════╗\n");
	printf("║                                                            ║\n");
	printf("║                                           NO.1200012850    ║\n");
	printf("║                                                            ║\n"); 
	printf("║     OTHELLO GAME PROGRAMED BY PATRICIA.XIAO                ║\n"); 
	printf("║     黑白棋人机对战小游戏 肖之屏出品                        ║\n"); 
	printf("║                                                            ║\n"); 
	printf("║     Welcome to play Othello with me~                       ║\n"); 
	printf("║     Enjoy yourself and have a great time~~~                ║\n"); 
	printf("║     欢迎使用简易版黑白棋对战小程序 祝您玩得愉快 ~~~        ║\n"); 
	printf("║                                                            ║\n");
	printf("║            1.RULES        规则说明                         ║\n"); 
	printf("║            2.NEW START    新开始                           ║\n"); 
	printf("║            3.LOAD         读盘                             ║\n"); 
	printf("║            4.SAVE         存盘                             ║\n"); 
	printf("║            0.END & EXIT   结束                             ║\n");
	printf("║                                                            ║\n");
	printf("╚══════════════════════════════╝\n"); 
	printf("\tSelet a choice from the above, please:\n");
	printf("\t请输入0-4中的某个数字，或菜单首字母：\n\n"); 
}

int print_rule()
{
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
		printf("\tAfter reading the introduction, what's your decision next?\n");
		printf("\t明白了的话请选择继续游戏，还是退出游戏：\n");
		printf("\t1.Continue 继续游戏\n \t0.Leave    退出游戏\n");

		char ch;
		getchoice( ch, 4, "1c0l 操作选择（Selection）:\n1=继续（Continue, C）  0=退出（Leave，L）" );
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
	printf("\n\n  游戏结束~~~\n  现在公布比赛结果：\n\n");
	printf("  最终结果：\n\n\t  您的子剩余%d个，计算机的子剩余%d个！\n\n",h,c);
	if(h<c)
	    printf("  很遗憾您输了！但是请别气馁，对手可是计算机，您已经很厉害啦！！\n");
	else
		if(h>c)
			printf("  天哪，连计算机都下不赢您，您这是要逆天啊！给大神跪了！\n");
		else
		    printf("  平手啊~~~难分胜负的说~~~不然再来一盘吧~~~\n");
    printf("                                                                ║\n");
	printf("                                                            ══╝\n");
}


//判断是否合法棋盘的系列函数
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
				temp[i][j]=1;
			else
				if(a[i][j]==EMPTY)
				    temp[i][j]=0;
				else								// 存在非法字符
					return IllegalChar;
		}
	}
	for(i=SCALE/2-1;i<=SCALE/2;i++)
	{
		for(j=SCALE/2-1;j<=SCALE/2;j++)
			if(!temp[i][j])							// 如果中心没有棋子，一定非法
				return NoCentralChess;
	}
	walk(SCALE/2,SCALE/2,temp);
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
			if(temp[i][j])
				return NotConnected;				// 未连成一片，一定不行
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

//判断能否落子的函数
int judge_put(int i, int j, char whoseturn, char board[SCALE][SCALE])
{
    if(board[i][j]!=EMPTY)								// 不能重叠放子
		return 0;
	char against;
	if(whoseturn==COMPUTER)
		against=HUMAN;
	else
		against=COMPUTER;
	int k;
	if(board[i][j]!=EMPTY)								// 不能重叠放子
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
		for(j=0;j<SCALE;j++)
		{
			human[i][j]=computer[i][j]=0;
			if(judge_put(i,j,HUMAN,board))
				human[i][j]=1;
			if(judge_put(i,j,COMPUTER,board))
				computer[i][j]=1;
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
	else  // bug= =b没加大括号（我就是懒了一下，至于吗……）
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
	// 左下右上
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


//估值函数
//给当前棋局估值
//寻找最优点落棋的系列函数

void c_val(char board[SCALE][SCALE])   //计算可变权重
{
	int i,j;
	int n=SCALE-1;
	for(i=0;i<=n;i++)
		for(j=0;j<=n;j++)
			change_value[i][j]=0;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// 上
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// 下
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	if((board[0][0]==COMPUTER)||(board[0][n]==COMPUTER))	// 左
		for(i=0;i<=n;i++)
			change_value[0][i]=500;
	if((board[n][0]==COMPUTER)||(board[n][n]==COMPUTER))	// 右
		for(i=0;i<=n;i++)
			change_value[n][i]=500;
	// 对方占了角的情况（粗略处理）
	if(board[0][0]==HUMAN)
	    change_value[0][1]=change_value[1][0]=change_value[1][1] =- 300;
	if(board[0][SCALE-1]==HUMAN)
	    change_value[0][SCALE-2]=change_value[1][SCALE-2]=change_value[1][SCALE-1] =- 300;
	if(board[SCALE-1][0]==HUMAN)
	    change_value[SCALE-1][1]=change_value[SCALE-2][0]=change_value[SCALE-2][1] =- 300;
	if(board[SCALE-1][SCALE-1]==HUMAN)
	    change_value[SCALE-1][SCALE-2]=change_value[SCALE-2][SCALE-1]=change_value[SCALE-2][SCALE-2] =- 300;
	// 对方占边的策略变化粗处理
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
	//即将占角的情况粗略处理
	int fight = 1000;
	//origin:300
	//左上角
	if(board[0][1]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][0]!=EMPTY)
		change_value[0][0]+=fight;
	if(board[1][1]!=EMPTY)
		change_value[0][0]+=fight;
	//右上角
	if(board[0][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-2]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	if(board[1][SCALE-1]!=EMPTY)
		change_value[0][SCALE-1]+=fight;
	//左下角
	if(board[SCALE-1][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][1]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	if(board[SCALE-2][0]!=EMPTY)
		change_value[SCALE-1][0]+=fight;
	//右下角
	if(board[SCALE-2][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-1][SCALE-2]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
	if(board[SCALE-2][SCALE-1]!=EMPTY)
		change_value[SCALE-1][SCALE-1]+=fight;
}


//估值函数
//给当前棋局估值

int temp_value(char board[SCALE][SCALE])		// 对低中高级有不同的计算权重方法
{
	for(int i = 0 ; i < SCALE ; i++)
		for(int j = 0 ; j < SCALE ; j++)
			change_value[i][j] = 0;
	if ( DEPTH == HIGH )						// 棋力是中低级就不计算动态权重（只计算高级）
		c_val(board);
	int score=0;
	int i,j;
	for(i=0;i<SCALE;i++)
	{
		for(j=0;j<SCALE;j++)
		{
			if(board[i][j]==COMPUTER)  
			{
				if(DEPTH != LOW)		// 若低级棋力则在计算机下的时候不计算稳定性。让玩家开心
					score+=steady(i,j,COMPUTER,board)*30;	// 稳定性
				score+=(const_value[i][j]+change_value[i][j]);      //位置固有权重+可变权重
			}
			else
				if(board[i][j]==HUMAN)
				{
					score-=steady(i,j,HUMAN,board)*30;
					score-=(const_value[i][j]-change_value[i][j]);  //注意change_value全是以计算机角度为准的！
				}
			if(DEPTH != LOW)
			{
				if(judge_put(i,j,COMPUTER,board))			// 行动力
					score+=100;
				if(judge_put(i,j,HUMAN,board))
					score-=100;
			}
			else
			{
				int num_c = 1000; 
				int num_h = 1000;
				//origin:400//origin:500//origin:600
				if(judge_put(i,j,COMPUTER,board))			// 行动力
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

// 加alpha-beta剪枝
long long find_position(int depth, char last_board[SCALE][SCALE], long long alpha, long long beta)
{
	long long temp;										// 分数
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
			if(judge_put(i,j,now,last_board))			// 可以落子之处
			{
				change_color(i,j,now,temp_board);
				temp=find_position(depth+1, temp_board, alpha, beta);
				for(p=0;p<SCALE;p++)
					for(q=0;q<SCALE;q++) 
					{
						temp_board[p][q]=last_board[p][q];	// 复原
					}
				if(depth%2==0)							// 该机器走
				{
					if(a == 0)							// 只是为了确保有解
														// 当然这样也许可以说是不完全的αβ剪枝
														// 因为我的min - max结构本来也不是很正统。
				    {
					    alpha = temp;						// 好吧我没说真话，真话是我的评估函数好夸张能超出整型范围的。
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
							comp_y = temp_y;				// 看来，问题是处在这里啊？不爱用指针伤不起……
						}
						return alpha;
					}
				}
                else
				{
					if(beta>temp)						// 人的一方
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