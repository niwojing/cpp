#include<iostream>
#include<graphics.h>
#include"tools.h"
#include<cstring>
using namespace std;
enum {wandou,xiangrikui,zhiwucount};
IMAGE imgbackground;
IMAGE imgbar;
IMAGE card[zhiwucount];
IMAGE *imgzhiwu[zhiwucount][20];
int curx,cury;//当前鼠标位置
int curzhiwu;//0代表没有，几就代表第几个

class zhiwu {
public:
	int type; //0代表没有，1代表第一种
	int frameindex;//第几帧
};

zhiwu map[5][9];
bool file(const char *name){
	FILE* fp = fopen(name, "r");
	if (fp==NULL) { return false; }
	else { fclose(fp); return true; }
}

void gameinit() {
	//加载背景工具栏
	loadimage(&imgbackground, ("D:\\vs\\code\\dazuoye\\resgit\\Map\\map0.jpg"));
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
			sprintf_s(name, sizeof(name), "resqq/res/zhiwu/%d/%d.png",i,j+1);
			if (file(name)) {
				imgzhiwu[i][j] = new IMAGE;
				loadimage(imgzhiwu[i][j], name);
			}
			else {
				break;
			}
		}
	}
	//创建窗口
	initgraph(1400, 600);
	curzhiwu = 0;
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
			int row = (msg.y - 90) / 120;
			int col = (msg.x - 320) / 100;
			cout << row << " " << col<<endl;
			cout << msg.y << " " << msg.x << endl;
			status = 0;
			curzhiwu = 0;
		}
	};
}

int main() {
	gameinit();
	while (1) {

		click();

		updatewindow();
	}
	
	system("pause");
	return 0;
}