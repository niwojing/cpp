#include<iostream>
#include<graphics.h>
#include"tools.h"
#include<cstring>
#include<time.h>
using namespace std;
enum {wandou,xiangrikui,zhiwucount};
IMAGE imgbackground;
IMAGE imgbar;
IMAGE card[zhiwucount];
IMAGE *imgzhiwu[zhiwucount][20];
IMAGE imgsunshineball;
int curx,cury;//当前鼠标位置
int curzhiwu;//0代表没有，几就代表第几个

class zhiwu {
public:
	int type; //0代表没有，1代表第一种
	int frameindex;//第几帧
};
zhiwu map[5][9];
class sunshineball {
public:
	int x, y;//飘落位置
	int destY;//目标y坐标
	bool used;
	int timer;
};
sunshineball balls[10];

bool file(const char *name){
	FILE* fp = fopen(name, "r");
	if (fp==NULL) { return false; }
	else { fclose(fp); return true; }
}
void gameinit() {
	//加载背景工具栏
	loadimage(&imgbackground, ("D:\\vs\\code\\dazuoye\\res\\bg1.jpg"));
	loadimage(&imgbar, ("D:\\vs\\code\\dazuoye\\resgit\\bar5.png"));
	memset(imgzhiwu, 0, sizeof(imgzhiwu));
	memset(map, 0, sizeof(map));
	//创建植物卡牌
	char name[64];
	for (int i = 0; i < zhiwucount; i++)
	{
		sprintf_s(name, sizeof(name), "resgit/Cards/card_%d.png", i + 1);
		loadimage(&card[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i,j+1);//植物摇摆
			if (file(name)) {
				imgzhiwu[i][j] = new IMAGE;
				loadimage(imgzhiwu[i][j], name);
			}
			else {
				break;
			}
		}
	}
	/*memset(balls, 0, sizeof(balls));
	sprintf_s(name, sizeof(name),"res/sunshine/1.png");//p9  15min*/
	loadimage(&imgsunshineball,"D:\\vs\\code\\dazuoye\\res\\sunshine\\1.png" );
	
	initgraph(900, 600);//创建窗口
	curzhiwu = 0;
	srand(time(NULL));
}
void updatewindow() {
	BeginBatchDraw();//双缓冲
	putimage(0, 0, &imgbackground);
	putimagePNG(250, 0, &imgbar);

	for (int i = 0; i < zhiwucount; i++)//植物放在卡槽里
	{
		int x = 338+i*65;
		int y = 6;
		putimage(x, y, &card[i]);
	}
	//拖动植物跟随鼠标移动
	if (curzhiwu > 0) {//说明已经选中植物
		IMAGE* img = imgzhiwu[curzhiwu - 1][0];

		putimagePNG(curx-0.5*img->getwidth(), cury-0.5*img->getheight(), img);
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				int x = 256 + j * 81;
				int y = 179 + i * 102;
				int zhiwutype = map[i][j].type - 1;
				int index = map[i][j].frameindex;
				putimagePNG(x, y, imgzhiwu[zhiwutype][index]);
			}
		}
	}
	int ballmax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballmax; i++) {
		if (balls[i].used) {
			IMAGE* img =&imgsunshineball;
			putimagePNG(balls[i].x, balls[i].y, img);
		}
	}
	EndBatchDraw();
	//结束双缓冲 PS：双缓冲能有效防止由于循环带来的画面不停闪烁及卡顿
	//先把所有元素加载好再一次性输出到屏幕
}
void click() {
	ExMessage msg;//easyX自带类，用于判断是否有鼠标事件
	static int status = 0;//0表示没选植物，1代表选中
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 338 && msg.x < 338 + zhiwucount * 65&&msg.y<96)//点击植物栏内
			{
				int index = (msg.x - 338) / 65;//index确定第几张卡
				//cout << index;
				curzhiwu = index+1;
				status = 1;
			}
		}
		else if(msg.message==WM_MOUSEMOVE&&status==1){
			curx = msg.x;
			cury = msg.y;
		}
		else if (msg.message == WM_RBUTTONDOWN&&status==1) {
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				if (map[row][col].type == 0) {
					map[row][col].type = curzhiwu;
					map[row][col].frameindex = 0;
				}
			}
			/*cout << row << " " << col << endl;
			cout << msg.y << " " << msg.x << endl;*/
			status = 0;
			curzhiwu = 0;
		}
	};
}
void creatsunshine() {
	static int count = 0;
	static int fre = 200;
	count++;
	if (count >= fre) {
		fre = 200 + rand() % 200;
		count = 0;
		int ballmax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballmax && balls[i].used; i++) {
			if (i >= ballmax) { return; }
		}
		balls[i].used = true;
		balls[i].x = 260 + rand() % 640;
		balls[i].y = 60;
		balls[i].destY = 200 + (rand() % 4) * 90;
		balls[i].timer = 0;
	}
}
void updatesunshine() {
	int ballmax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballmax; i++) {
		if (balls[i].timer == 0) {
			balls[i].y += 2;
		}
		if (balls[i].y >= balls[i].destY) {
				
			balls[i].timer++;
			if (balls[i].timer > 100) {
				balls[i].used = false;
			}
		}
		
	}
}
void updategame() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameindex++;
				int zhiwuType = map[i][j].type - 1;
				int index = map[i][j].frameindex;
				if (imgzhiwu[zhiwuType][index] == NULL) {
					map[i][j].frameindex = 0;
				}
			}
		}
	}
	creatsunshine();
	updatesunshine();
}

void startui() {
	IMAGE imgbg,imgmenu1,imgmenu2;
	loadimage(&imgbg, "resgit/menu.png");
	loadimage(&imgmenu1, "resgit/menu1.png");
	loadimage(&imgmenu2, "resgit/menu2.png");
	int flag = 0;
	while (1) {
		BeginBatchDraw();
		putimagePNG(0, 0, &imgbg);
		putimagePNG(475, 75, flag ? &imgmenu2 : &imgmenu1);
		
		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN&& msg.x > 474
				&& msg.x < 774 && msg.y>75 && msg.y < 215) {
				flag = 1;
			}
			else if (msg.message == WM_LBUTTONUP&&flag) {
				return;
			}
		}
		EndBatchDraw();
	}
}




int main() {
	gameinit();
	startui();
	int timer = 0;
	bool flag = true;
	while (1) {

		click();
		timer += getDelay();
		if (timer > 35) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			
			updategame();
			updatewindow();
		}
	}
	
	system("pause");
	return 0;
}