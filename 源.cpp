#include<stdio.h>
#include<easyx.h>
#include<conio.h>
#include<stdlib.h>
#include<time.h>
#include "tools.cpp"
#pragma comment(lib,"MSIMG32.LIB")
#pragma comment(lib, "winmm.lib")//静态库加载
#pragma warning(disable:4996)//sprintf函数报错
#define WIN_H 600
#define WIN_W 900
#define Sunw 80
#define Sunh 80
#define SUNV 3
#define TIM2 8
#define SUNTIM1 300//产生阳光间隔
#define SUNWAN 100//豌豆阳光消耗
#define SUNXG 50//阳光消耗
#define SUNNUM 10//阳光数组总量
#define ZMBINUM 6//僵尸数组量
#define ZMBIALL 30//僵尸总量
#define ZMBI1 5//第一波僵尸量
#define ZMBI2 8//第二波僵尸量  注意第一波与第二波僵尸量和要小于僵尸总量
#define ZMBITIM3 2
#define ZMBITIM2 6
#define ZMBITIM1 16//僵尸产生时间第一波
#define ZMBIHP 5//僵尸血量
#define ZMBIV 1//僵尸速度
#define ZHIHP 100//植物血量
#define BULLETV 3//子弹速度
#define BULLETTIME 150//子弹间隔 与阳光间隔
#define SUNACTION 50//初始阳光量
#define BULLETNUM 30//子弹数量上线
#define CFRE 4//帧数控制植物
#define SUNCFRE 3//太阳帧数控制
#define ZMBICFRE 6//僵尸帧数控制
struct function {
	int x=0;
	int y=0;
	int exit=0;
};
struct zhiwu {
	int hp = ZHIHP;
	int exit = 0;
	int cls = 1;
	int frame = 1;
	int cfre = CFRE;
	int time= BULLETTIME;
};
struct sunshine {
	int exit=0;
	int x=0;
	int y=0;//左上角
	int sunnum = 25;
	int zhi = 0;//自然光1植物光0
	int  frame = 1;
	int cfre = SUNCFRE;
};
struct zmbi {
	int hp = ZMBIHP;
	int line = 0;//0  1 2
	int x = WIN_W;
	int y =0;
	int i=0;
	int j=0;
	int eat = 0;//是否处于吃的状态
	int exit=0;
	int frame = 1;
	int cfre = ZMBICFRE;
};
zmbi zmb[ZMBINUM];
int countkill = 0;
int  countzm = 0;//僵尸产生总量
sunshine sun[SUNNUM];
int sunsum = SUNACTION;//阳光初始化
function fun[BULLETNUM];
char Sunsum[4];
zhiwu wei[3][9];
char graph[20];
int zmbiline[3] = { 0,0,0 };//哪行有僵尸
ExMessage msg;
void zmbieat();
int zhiwux(int j);
void juzmbi();
void probullet();
void zz(ExMessage msg, int cls);
void control();
void producezmbi();
void displayzmbi();
void BGMbk();
void initgame();
void dissunnum();
void move(int x, int y, int c);
void mousecad();
int janc(int i, int j, ExMessage msg);
void plandisplay();
void producesun();
int zmbiwei();
void upgame();
void gameprompt();
int main() {
	initgame();
	while (1) {
		peekmessage(&msg, EX_MOUSE);
		if (msg.x < 788 && msg.x>488 && msg.y < 210 && msg.y>72 && msg.message == WM_LBUTTONDOWN) {
			break;
		}
	}
	//开始游戏
	IMAGE w;
	IMAGE l;
	int win = 2;//胜利判断
	IMAGE bk, br, cr1, cr2;
	BGMbk();
	while (1) {
		/*control();*/
		/*upgame();*/
		/*IMAGE bk, br, cr1, cr2;*/
		producezmbi();//产生僵尸
		zmbieat();
		BeginBatchDraw();
		sprintf(Sunsum, "%d", sunsum);//转换
		loadimage(&bk, "./bg改.jpg", 0, 0);
		putimage(0, 0, &bk);
		plandisplay();//显示植物
		displayzmbi();//刷新僵尸
		probullet();//产生子弹
		juzmbi();//判断僵尸所在行
		producesun();
		loadimage(&br, "./bar4.png", 0, 0);
		putimage(200, 0, &br);//卡槽
		dissunnum();//显示阳光
		loadimage(&cr1, "./card_1.png", 0, 0);
		putimage(270, 0, &cr1);//第一个卡片
		loadimage(&cr2, "./card_2.png", 0, 0);
		putimage(332, 0, &cr2);//卡片
		mousecad();
		flushmessage();//清空缓存  豌豆不能放置
		gameprompt();
		FlushBatchDraw();
		if (countkill >= ZMBIALL) {
			win = 1;
			break;
		}
		else if (zmbiwei() == 1) {
			win = 0;
			break;
		}//游戏结束
		system("cls");
		/*	FlushMouseMsgBuffer();*/
	}
	//结束结算
	BeginBatchDraw();
	if (win == 1) {
		mciSendString("close gamebk", 0, 0, 0);
		mciSendString("play zhi/mci/winmusic.mp3", 0, 0, 0);
		loadimage(&w, "zhi/界面/win2.png", 0, 0);
		putimage(0, 0, &w);
	}
	else if (win == 0) {
		mciSendString("close gamebk", 0, 0, 0);
		mciSendString("play zhi/界面/lose.mp3", 0, 0, 0);
		loadimage(&l, "zhi/僵尸吃掉的脑子.png", 0, 0);
		putimagePNG(200,100 , &l);
	}
	FlushBatchDraw();
	system("pause");
}
//使程序每10刷一帧
//int main() {
//	initgame();
//	IMAGE img;
//	int x = 0, y = 0;
//	loadimage(&img, "zhi/界面/Adventure_0.png", 300, 140);
//
//	while (1) {
//		system("cls");
//		BeginBatchDraw();
//		IMAGE ac;
//		loadimage(&ac, "zhi/界面/menu.png", 0, 0);
//		putimage(0, 0, &ac);
//		upgame();
//		/*settextstyle(75, 25, "Consolas");
//        outtextxy(x, y, "开始游戏");*/
//		putimagePNG(x, y, &img);
//		mousecad();
//		if (GetAsyncKeyState(VK_UP))y -= 4;
//		if (GetAsyncKeyState(VK_DOWN))y += 4;
//		if (GetAsyncKeyState(VK_LEFT))x -= 4;
//		if (GetAsyncKeyState(VK_RIGHT))x += 4;
//		FlushBatchDraw();
//		printf("x=%d  y=%d\n", x, y);
//	}
//	system("puase");
//}
void gameprompt() {
	int static countext = 0;
	int static outext = 0;
	int static diyi = 1;
	if (countzm == ZMBI1 || outext == 1 && diyi != 0) {
		outext = 1;
		countext++;
		if (countext == 50) {
			countext = 0;
			outext = 0;
			diyi = 0;
		}
		settextstyle(50, 25, "Consolas");
		if (countext == 1) {
			mciSendString("play zhi/mci/hugewave.mp3", 0, 0, 0);
		}
		outtextxy(230, 100, "一大波僵尸即将袭来！");
	}
	else if (countzm == ZMBI1 + ZMBI2 || outext == 1) {
		outext = 1;
		countext++;
		if (countext == 50) {
			countext = 0;
			outext = 0;
		}
		settextstyle(50, 25, "Consolas");
		if (countext == 1) {
			mciSendString("play zhi/mci/hugewave.mp3", 0, 0, 0);
		}
		outtextxy(230, 100, "最后一波僵尸即将袭来！");
	}
}
int zmbiwei() {
	int r = 0;
	for (int i = 0; i < ZMBINUM; i++) {
		if (zmb[i].x <= 0) {
			r = 1;
			break;
		}
	}
	return r;
}//用于判断游戏结束
void zmbieat() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			for (int n = 0; n < ZMBINUM; n++) {
				if (zmb[n].exit == 1 && i == zmb[n].line &&wei[i][j].exit==1) {
					if (zhiwux(j) >= zmb[n].x + 60 - 20 && zhiwux(j) <= zmb[n].x + 60 + 20) {
						zmb[n].eat = 1;
						zmb[n].i = i;
						zmb[n].j = j;//标记吃的哪一个 可以不写
						wei[i][j].hp--;//扣血
						if (wei[i][j].hp <= 0) {
							mciSendString("play zhi/mci/bigchomp.mp3", 0, 0, 0);
							wei[i][j].exit = 0;
						/*	zmb[n].eat = 0;*/
						}//植物死亡
					}
				}//条件筛选
			}
		}
	}
	for (int n = 0; n < ZMBINUM; n++) {
		if (zmb[n].eat == 1 && wei[zmb[n].i][zmb[n].j].exit == 0) {
			zmb[n].eat = 0;
		}
	}//写到上面的for中也可以
}//
//遍历僵尸遍历植物当x坐标小于等(按行计算)时  吃  停止  扣血
// 吃 设置行走和吃的状态
// 扣血   设置帧数
// 僵尸吃植物
//
int zhiwux(int j) {
	return 150 + j * 80;
}//算出植物x坐标
void juzmbi(){
	int i,r0=0,r1=0,r2=0;
	for (i = 0; i < ZMBINUM; i++) {
		if (zmb[i].exit == 1) {
			zmbiline[zmb[i].line] = 1;
		}
		if (zmb[i].line == 0 && zmb[i].exit == 1&&r0!=1) {
			r0 = 1;
		}if (zmb[i].line == 1 && zmb[i].exit == 1 && r1 != 1) {
			r1 = 1;
		}if (zmb[i].line == 2 && zmb[i].exit == 1 && r2 != 1) {
			r2 = 1;

		}
	}
	if (r0 == 1) {
	}
	else zmbiline[0] = 0;
	if (r1 == 1) {
	}
	else zmbiline[1] = 0;
	if (r2 == 1) {
	}
	else zmbiline[2] = 0;
	
}//判断僵尸出现行赋到zmbiline
void probullet() {
	int static count = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (zmbiline[i] == 1 &&wei[i][j].exit==1&&wei[i][j].cls==1) {
				if (wei[i][j].time >= BULLETTIME) {
					wei[i][j].time = 0;
					fun[count].exit = 1;
					fun[count].x = 200 + j * 80;
					fun[count].y = 100+100 * (i + 1);
					count++;
					if (count >= BULLETNUM)count = 0;
				}else wei[i][j].time++;
			}
		}
	}//产生子弹
	IMAGE img, imgp;
	loadimage(&img, "bullet.png", 0, 0);
	loadimage(&imgp, "zhi/zhiwu/0/PeaBulletHit.gif", 0, 0);
	for (int i = 0; i < BULLETNUM; i++) {
		if (fun[i].exit == 1) {
			putimagePNG(fun[i].x, fun[i].y, &img);
			fun[i].x += BULLETV;
			if (fun[i].x >= WIN_W &&fun[i].exit==1) {
				fun[i].exit = 0;
			}//消除子弹终止
		}//刷新子弹
		for (int j = 0; j < ZMBINUM; j++) {
			if (fun[i].y == 200 && fun[i].exit == 1 && zmb[j].line == 0 &&zmb[j].exit==1) {
				if (zmb[j].x <= fun[i].x - 70 + 5 && zmb[j].x >= fun[i].x - 70 - 50) {
					fun[i].exit = 0;
					zmb[j].hp--;
					putimagePNG(fun[i].x, fun[i].y, &imgp);
					mciSendString("play zhi/wan.mp3", 0, 0, 0);
					if (zmb[j].hp <= 0) {
						zmb[j].exit = 0;
						countkill++;
					}
				}
			}
			if (fun[i].y == 300 && fun[i].exit == 1 && zmb[j].line == 1  && zmb[j].exit == 1) {
				if (zmb[j].x <= fun[i].x - 70 + 5 && zmb[j].x >= fun[i].x - 70 - 50) {
					fun[i].exit = 0;
					zmb[j].hp--;
					putimagePNG(fun[i].x, fun[i].y, &imgp);
					mciSendString("play zhi/wan.mp3", 0, 0, 0);
					if (zmb[j].hp <= 0) {
						zmb[j].exit = 0;
						countkill++;
					}
				}
			}
			if (fun[i].y == 400 && fun[i].exit == 1 && zmb[j].line == 2 && zmb[j].exit == 1) {
				if (zmb[j].x <= fun[i].x - 70 + 5&& zmb[j].x >= fun[i].x - 70 - 50) {
					fun[i].exit = 0;
					zmb[j].hp--;
					putimagePNG(fun[i].x, fun[i].y, &imgp);
					mciSendString("play zhi/wan.mp3", 0, 0, 0);
					if (zmb[j].hp <= 0) {
						zmb[j].exit = 0;
						countkill++;
					}
				}
			}
		}
	}
	//for (i = 0; i < 3; i++) {
	//	for (j = 0; j < 9; j++) {
	//		if (wei[i][j].cls == 1 && zmbiline[i] == 1 &&wei[i][j].exit==1) {
	//			wei[i][j].fun[count].exit = 1; 
	//			wei[i][j].fun[count].x = wei[i][j].acx;//初始x
	//			count++;
	//			if (count >= BULLETNUM) {
	//				count = 0;
	//			}
	//			if ((te - time(NULL)) % BULLETTIME == 0) {
	//				wei[i][j].fun[count].exit = 1;
	//				wei[i][j].fun[count].x = wei[i][j].acx;//初始x
	//			}//子弹产生
	//			for (n = 0; n < BULLETNUM; n++) {
	//				if (wei[i][j].fun[n].exit == 1) {
	//					
	//					wei[i][j].fun[n].x += BULLETV;//显示子弹
	//					if (wei[i][j].fun[n].x >= WIN_W) {
	//						wei[i][j].fun[n].exit = 0;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}//产生并刷新子弹攻击僵尸
//1植物加time确定产生子弹间隔
//起始20后附零
//2遍历wei寻找条件
//3赋值子弹数组
//显示子弹
//1遍历子弹
//2按行分类
//3找出每行打头僵尸
//4消除子弹
//扣血
//添加僵尸死亡
void zz(ExMessage msg, int cls) {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 9; j++) {
			if (janc(i, j,msg) == 1 && wei[i][j].exit != 1)
			{
				/*mciSendString("play zhi/zz.mp3", 0, 0, 0);*/
				wei[i][j].cls = cls;
				wei[i][j].exit = janc(i, j,msg);
				wei[i][j].time = BULLETTIME;
				wei[i][j].hp = ZHIHP;
				mciSendString("play zhi/mci/plant.mp3", 0, 0, 0);
				if (cls == 1) {
					sunsum -= SUNWAN;
				}
				else sunsum -= SUNXG;
			}
		}

	}

}//种植对植物信wei息修改
void control() {
	/*for (int i = 0; i < 3; i++) {
		printf("%d\n", zmbiline[i]);
	}
	int co = 0;
	for (int i = 0; i < BULLETNUM; i++) {
		printf("%d  ", fun[i].exit);
		if (co == 6) {
			printf("\n");
			co = 0;
		}
		else co++;
	}
	printf("\n");
	int co1 = 0;
	for (int i = 0; i < BULLETNUM; i++) {
		printf("%d  ", fun[i].y);
		if (co1 == 6) {
			printf("\n");
			co1 = 0;
		}
		else co1++;
	}
	printf("\n");*/
	/*int count=0;
	for (int i = 0; i < ZMBINUM; i++) {
		printf("%d  ", zmb[i].hp);
		if (count == 9) {
			count = 0;
			printf("\n");
		}
		else count++;
	}*/
	/*for (int i = 0; i < ZMBINUM; i++) {
		printf("%d  ", zmb[i].eat);
	}*/
}
void producezmbi() {
	int static te= time(NULL);
	int static count = 0;//记录僵尸数组序号
	if (zmb[count].exit != 1) {
		if ((time(NULL) - te + 1) % ZMBITIM1 == 0 && countzm < ZMBI1) {
			zmb[count].exit = 1;//僵尸初始化
			zmb[count].hp = ZMBIHP;
			zmb[count].x = WIN_W;
			zmb[count].eat = 0;
			zmb[count].line = rand() % 3;
			srand(time(NULL));
			zmb[count].y = 120 + zmb[count].line * 102 + 12;
			countzm++;//产生量加一
			count++;//记录量加一
			if (count >= 6) {
				count = 0;
			}
			te = time(NULL);
		}
		else if ((time(NULL) - te + 1) % ZMBITIM2 == 0 && ZMBI1 <= countzm && countzm < ZMBI2 + ZMBI1) {
			zmb[count].exit = 1;//僵尸初始化
			zmb[count].hp = ZMBIHP;
			zmb[count].x = WIN_W;
			zmb[count].eat = 0;
			zmb[count].line = rand() % 3;
			srand(time(NULL));
			zmb[count].y = 120 + zmb[count].line * 102 + 12;
			countzm++;//产生量加一
			count++;//记录量加一
			if (count >= 6) {
				count = 0;
			}
			te = time(NULL);
		}
		else if ((time(NULL) - te + 1) % ZMBITIM3 == 0 && ZMBI2 + ZMBI1 <= countzm && countzm < ZMBIALL) {
			zmb[count].exit = 1;//僵尸初始化
			zmb[count].hp = ZMBIHP;
			zmb[count].x = WIN_W;
			zmb[count].eat = 0;
			zmb[count].line = rand() % 3;
			srand(time(NULL));
			zmb[count].y = 120 + zmb[count].line * 102 + 12;
			countzm++;//产生量加一
			count++;//记录量加一
			if (count >= 6) {
				count = 0;
			}
			te = time(NULL);
		}
	}//判断僵尸数组是否充满
	
}//产生僵尸分成三波各自有对应时间对应数量
void displayzmbi() {
	int i;
	for (i = 0; i < ZMBINUM; i++) {
		if (zmb[i].exit == 1) {
			IMAGE imgzmbi;
			if (zmb[i].eat == 0) {
				if (zmb[i].cfre == ZMBICFRE) {
					zmb[i].cfre = 0;
					zmb[i].frame++;
					if (zmb[i].frame++ >= 11)zmb[i].frame = 1;
				}
				else zmb[i].cfre++;
				sprintf(graph, "zhi/zmgo/%d.png", zmb[i].frame);
				loadimage(&imgzmbi, graph, 0, 0);
				putimagePNG(zmb[i].x, zmb[i].y, &imgzmbi);
				zmb[i].x -= ZMBIV;

			}//移动
			else {
				if (zmb[i].cfre == ZMBICFRE) {
					zmb[i].cfre = 0;
					zmb[i].frame++;
					if (zmb[i].frame++ >= 21)zmb[i].frame = 1;
				}
				else zmb[i].cfre++;
				sprintf(graph, "zhi/zm_eat/%d.png", zmb[i].frame);
				loadimage(&imgzmbi, graph, 0, 0);
				mciSendString("play zhi/mci/chomp2.mp3", 0, 0, 0);
				putimagePNG(zmb[i].x, zmb[i].y, &imgzmbi);
			}//吃
		}
	}
}//刷新僵尸
void BGMbk() {
	mciSendString("open ./bg.MP3 alias gamebk", 0, 0, 0);
	mciSendString("play gamebk repeat", 0, 0, 0);
}
void initgame()//完成初始化 
{
	initgraph(WIN_W, WIN_H, SHOWCONSOLE);
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	IMAGE ac;
	loadimage(&ac, "zhi/界面/menu.png", 0, 0);
	putimage(0, 0, &ac);
	IMAGE img;
	loadimage(&img, "zhi/界面/Adventure_0.png", 300, 140);
	putimagePNG(488, 72, &img);
	/*mciSendString("open zhi/界面/Look up at the Sky.ogg", 0, 0, 0);*/
	//mciSendString("play zhi/界面/Look up at the Sky.ogg repeat", 0, 0, 0);//ogg格式不支持
}
void dissunnum() {
	settextstyle(20, 0, "Consolas");
	outtextxy(225,64,Sunsum);
}
void move(int x, int y, int c) {
	IMAGE ziwan, zixg;
	if (c == 1)
	{
		loadimage(&ziwan, "zhi/zhiwu/0/1.png", 0, 0);
		putimagePNG(x - 30, y - 60, &ziwan);
	}
	else if (c == 2)
	{
		loadimage(&zixg, "zhi/zhiwu/1/2.png", 0, 0);
		putimagePNG(x - 30, y - 60, &zixg);
	}
}//卡片拖动
void mousecad() {
	peekmessage(&msg, EX_MOUSE);
	int static wan = 0;
	int static moec2 = 0;
	if (wan == 1 || moec2 == 1 || msg.message == WM_LBUTTONDOWN) {
		if (moec2 == 1 || msg.x > 332 && msg.x < 390 && msg.y < 120 && msg.y>0) {
			if (sunsum >= 50) {
				move(msg.x, msg.y, 2);
				moec2 = 1;
				if (msg.message == WM_LBUTTONUP) {
					zz(msg , 2);
					moec2 = 0;
					printf("2置零");
				}
			}
		}//向日葵
		else if (wan == 1 || msg.x > 270 && msg.x < 332 && msg.y < 120 && msg.y>0) {
			if (sunsum >= 100) {
				move(msg.x, msg.y, 1);
				wan = 1;
				printf("1之一");
				if (msg.message == WM_LBUTTONUP) {
					zz(msg, 1);
					wan = 0;
					printf("1置零");
				}
			}
		}//豌豆
	}//卡片移动
		for (int i = 0; i < SUNNUM; i++) {
			if (sun[i].exit == 1 && msg.x > sun[i].x && msg.x < sun[i].x + Sunw && msg.y > sun[i].y && msg.y < sun[i].y + Sunh && msg.message == WM_LBUTTONDOWN) {
				mciSendString("play zhi/sun.mp3", 0, 0, 0);//有问题
				sun[i].exit = 0;
				sunsum += sun[i].sunnum;
			}//阳光收集
	    }
}
//void purge() {
//	if (count == 2) {
//		flushmessage();//清空缓存
//		count = 0;
//		return;
//	}
//	count++;
//}//清除缓存
int janc(int i, int j, ExMessage msg) {
	if (i < 0 || j < 0)printf("区域不在");
	int const x1 = 150 + j * 80, y1 = 180 + i * 100;
	if (msg.x > x1 && msg.x<x1 + 80 && msg.y>y1 && msg.y < y1 + 100) {
		return 1;
	}
	else return 2;
}//检测鼠标是否在种植区
void plandisplay() {
	int i, j;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 9; j++) {
			if (wei[i][j].exit == 1) {

				if (wei[i][j].cls == 1)
				{
					IMAGE ziwan;
					if (wei[i][j].cfre == CFRE) {
						wei[i][j].cfre = 0;
						wei[i][j].frame++;
						if (wei[i][j].frame++ >= 13)wei[i][j].frame = 1;
					}
					else wei[i][j].cfre++;
					sprintf(graph, "zhi/zhiwu/0/%d.png", wei[i][j].frame);
					loadimage(&ziwan, graph, 0, 0);
					putimagePNG(150 + j * 80, 180 + i * 100 + 12, &ziwan);
				}
				if (wei[i][j].cls == 2)
				{
					IMAGE zixg;
						if (wei[i][j].cfre == CFRE) {
							wei[i][j].cfre = 0;
							wei[i][j].frame++;
							if (wei[i][j].frame++ >= 18)wei[i][j].frame = 1;
					     }
					else wei[i][j].cfre++;
					sprintf(graph, "zhi/zhiwu/1/%d.png", wei[i][j].frame);
					loadimage(&zixg, graph, 0, 0);
					putimagePNG(150 + j * 80, 180 + i * 100 + 12, &zixg);
				}
			}
		}
	}
}//显示植物
void producesun() {
	int static count1 = 0;//自然光帧数
	int static count2 = 0;//植物光
	int static count = 0;//数组序数；
	for (int i = 0; i < SUNNUM; i++) {
		if (sun[i].exit == 1) {
			IMAGE imgsun;
			if (sun[i].zhi == 0) {
				if (sun[i].cfre == SUNCFRE) {
					sun[i].cfre = 0;
					sun[i].frame++;
					if (sun[i].frame++ >= 29)sun[i].frame = 1;
				}
				else sun[i].cfre++;
				sprintf(graph, "zhi/sunshine/%d.png", sun[i].frame);
				loadimage(&imgsun, graph, 0, 0);
				putimagePNG(sun[i].x, sun[i].y, &imgsun);
				if (sun[i].y < 300)sun[i].y += SUNV;//自然阳光
			}
			else if (sun[i].zhi == 1) {
					if (sun[i].cfre == SUNCFRE) {
						sun[i].cfre = 0;
						sun[i].frame++;
						if (sun[i].frame++ >= 29)sun[i].frame = 1;
					}
					else sun[i].cfre++;
					sprintf(graph, "zhi/sunshine/%d.png", sun[i].frame);
					loadimage(&imgsun, graph, 0, 0);
					putimagePNG(sun[i].x, sun[i].y, &imgsun);
			}
		}
	}//刷新阳光
	if (sun[count].exit == 0) {
		if (count1 % SUNTIM1 == 0) {
			count1 = 1;
			sun[count].exit = 1;
			sun[count].zhi = 0;
			srand(time(NULL));//刷新种子
			//x200---720   y0---300
			sun[count].x = rand() % 500 + 200;
			sun[count].y = 0;
			count++;
			if (count >= SUNNUM) {
				count = 0;
			}
		}
		else count1++;//自然光产生
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 9; j++) {
				if (wei[i][j].exit == 1 && wei[i][j].cls == 2) {
					if (wei[i][j].time == 0) {
						wei[i][j].time = 3*BULLETTIME;//向日葵产生间隔可改变
						sun[count].exit = 1;
						sun[count].x = 150 + j * 80;
						sun[count].y = 180 + i * 100 + 12;
						sun[count].zhi = 1;
						count++;
						if (count >= SUNNUM) {
							count = 0;
						}
					}
					else wei[i][j].time--;//植物光产生
				}
			}
		}
	}
	
}//产生阳光并刷新放到upgame;
void upgame() {
	IMAGE bk, br, cr1,cr2;
	sprintf(Sunsum, "%d", sunsum);//转换
	/*itoa(sunsum, Sunsum, 10);*/
	loadimage(&bk, "./bg改.jpg", 0, 0);
	putimage(0, 0, &bk);
	producezmbi();
	displayzmbi();//刷新僵尸
	plandisplay();//显示植物
	juzmbi();//判断僵尸所在行
	probullet();//产生子弹
	producesun();
	loadimage(&br, "./bar4.png", 0, 0);
	putimage(200, 0, &br);//卡槽
	dissunnum();
	loadimage(&cr1, "./card_1.png", 0, 0);
	putimage(270, 0, &cr1);//第一个卡片
	loadimage(&cr2, "./card_2.png", 0, 0);
	putimage(332, 0, &cr2);//卡片

}
