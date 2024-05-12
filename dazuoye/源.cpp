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
int curx,cury;//��ǰ���λ��
int curzhiwu;//0����û�У����ʹ���ڼ���

class zhiwu {
public:
	int type; //0����û�У�1�����һ��
	int frameindex;//�ڼ�֡
};
zhiwu map[5][9];
class sunshineball {
public:
	int x, y;//Ʈ��λ��
	int destY;//Ŀ��y����
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
	//���ر���������
	loadimage(&imgbackground, ("D:\\vs\\code\\dazuoye\\res\\bg1.jpg"));
	loadimage(&imgbar, ("D:\\vs\\code\\dazuoye\\resgit\\bar5.png"));
	memset(imgzhiwu, 0, sizeof(imgzhiwu));
	memset(map, 0, sizeof(map));
	//����ֲ�￨��
	char name[64];
	for (int i = 0; i < zhiwucount; i++)
	{
		sprintf_s(name, sizeof(name), "resgit/Cards/card_%d.png", i + 1);
		loadimage(&card[i], name);
		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i,j+1);//ֲ��ҡ��
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
	
	initgraph(900, 600);//��������
	curzhiwu = 0;
	srand(time(NULL));
}
void updatewindow() {
	BeginBatchDraw();//˫����
	putimage(0, 0, &imgbackground);
	putimagePNG(250, 0, &imgbar);

	for (int i = 0; i < zhiwucount; i++)//ֲ����ڿ�����
	{
		int x = 338+i*65;
		int y = 6;
		putimage(x, y, &card[i]);
	}
	//�϶�ֲ���������ƶ�
	if (curzhiwu > 0) {//˵���Ѿ�ѡ��ֲ��
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
	//����˫���� PS��˫��������Ч��ֹ����ѭ�������Ļ��治ͣ��˸������
	//�Ȱ�����Ԫ�ؼ��غ���һ�����������Ļ
}
void click() {
	ExMessage msg;//easyX�Դ��࣬�����ж��Ƿ�������¼�
	static int status = 0;//0��ʾûѡֲ�1����ѡ��
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 338 && msg.x < 338 + zhiwucount * 65&&msg.y<96)//���ֲ������
			{
				int index = (msg.x - 338) / 65;//indexȷ���ڼ��ſ�
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