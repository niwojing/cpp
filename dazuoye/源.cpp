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
int curx,cury;//��ǰ���λ��
int curzhiwu;//0����û�У����ʹ���ڼ���

class zhiwu {
public:
	int type; //0����û�У�1�����һ��
	int frameindex;//�ڼ�֡
};

zhiwu map[5][9];
bool file(const char *name){
	FILE* fp = fopen(name, "r");
	if (fp==NULL) { return false; }
	else { fclose(fp); return true; }
}

void gameinit() {
	//���ر���������
	loadimage(&imgbackground, ("D:\\vs\\code\\dazuoye\\resgit\\Map\\map0.jpg"));
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
	//��������
	initgraph(1400, 600);
	curzhiwu = 0;
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