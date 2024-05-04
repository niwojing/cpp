#include<iostream>
using namespace std;	//��ʱ��֪����ô�޸�
#include<graphics.h>
#include"tools.h"
#include<ctime>
#include<time.h>
#include<math.h>
#include"vector2.h"	//�������߰�

#include<mmsystem.h>	//����
#pragma comment(lib,"winmm.lib") 

enum {WAN_DAO,XIANG_RI_KUI,SHI_REN_HUA,ZHI_WU_COUNT};	//ֲ��ö��
enum {SUNSHINE_DOWN,SUNSHINE_GROUND,SUNSHINE_COLLECT,SUNSHINE_PRODUCT};  //������״̬ö��
enum { GOING, WIN, FAIL };

IMAGE imgBg;	//��Ϸ����ͼ
IMAGE imgBar;	//״̬������ֲ��ı�����
IMAGE imgCards[ZHI_WU_COUNT];	//ֲ�￨������
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//ֲ������ 

int curX, curY;	//��ǰѡ��ֲ�����ƶ��е�����
int curZhiWu;	//��ǰѡ�е�ֲ��	0-û��ѡ�У�1-ѡ�е�һ��ֲ��

int killZmCount;	//ɱ���Ľ�ʬ����
int zmCount;		//���ɵĽ�ʬ����
int gameStatus;		//��Ϸ��״̬

//�궨����Ϸ���ڴ�С
#define	WIN_WIDTH 900
#define	WIN_HEIGHT 600

#define ZM_MAX 10	//��ʬ����

struct zhiWu {	//ֲ��ṹ��
	int type;		//0-û��ֲ�1-��һ��ֲ��
	int frameIndex;	//����֡�����
	int shootTimer;	//ֲ�﹥��ʱ����
	bool catched;	//ֲ���Ƿ񱻽�ʬ����
	int deadTimer;	//ֲ�ﱻ��ʱ����������ʱ

	int x, y;
	int timer;	//�������տ���������ļ�ʱ��
};

struct sunShineBall {	//������ṹ��
	int	x, y;//�������x��y����
	int	frameIndex;	//����������֡�����
	int	destY;	//������ֹͣ��y����
	bool used;	//�������Ƿ���ʹ��
	int timer;	//��ʱ����������������������ͣ��ʱ��

	int xoff;	//�������λ��x����
	int yoff;	//�������λ��y����

	//�Ż�
	float t;	//���������ߵ�ʱ���	0��1
	vector2 p1, p2, p3, p4;
	vector2	pCur;	//��ǰʱ���������λ��
	float	speed;
	int	status;	//�������״̬
};

struct sunShineBall	balls[10];	//������أ��������ȴ洢����
IMAGE	imgSunShineBall[29];	//��������֡����	-	���Զ���һ���꣬������ڹ���

struct zm {		//��ʬ�ṹ��	-���ڻ�����Ҫ��ֲ��һ�����ö��,���㴴����ͬ���͵Ľ�ʬ
	int x, y;
	int row;
	int frameIndex;
	bool used;
	int speed;	//��ʬǰ���ٶ�
	int blood;	//��ʬѪ��
	bool dead;	//��ʬ�Ƿ�����
	bool eating;	//��ʬ�Ƿ��ڳ�ֲ��
};

struct zm zms[10];	//��ʬ��,�������ȴ洢��ʬ
IMAGE	imgZm[22];	
IMAGE	imgZmDead[20];
IMAGE	imgZmEat[21];
IMAGE	imgZmStand[11];

struct bullet {	//�㶹�ӵ��ṹ��
	int x, y, row,speed;
	bool used;
	bool blast;	//�Ƿ�ը
	int frameIndex;	//��ը֡���
};

struct bullet bullets[30];	//�㶹�ӵ���
IMAGE imgBulletNormal;
IMAGE imgBulletBlast[4];

int ballMax = sizeof(balls) / sizeof(balls[0]);	//����������������	-�ǲ��Ǻ���֣����������Ѿ�������10��
												//���ﻹҪ����һ�飬���ʣ��ʾ��Ǹ߿���(���˽����10����һ���궨�壬���Ӹ߿���)
int zmMax = sizeof(zms) / sizeof(zms[0]);	//��ʬ���н�ʬ������

int bulletMax = sizeof(bullets) / sizeof(bullets[0]);	//�㶹�ӵ��ص�����

struct zhiWu map[3][9];	//��ͼ���飬����洢ֲ��

int sunShine;	//����ֵ

//������Ϸ��ʼ��
void gameInit();

//������Ϸ��ʼ����
void startUI();

//��������Ѳ��
void viewScence();

//����״̬���»�
void barsDown();	

//������Ⱦ��Ϸ����(��ȾͼƬ��������)
void updateWindow();

//�����û��������������
void userClick();

//�����ж��ļ��Ƿ����
bool fileExist(const char* name);

//�������¶���
void updateGame();

//���������Ϸ�Ƿ����
bool checkOver();

//������������
void createSunShine();

//�����������⶯��
void updateSunShine();

//�����ռ�����
void collectSunshine(ExMessage* msg);

//����������ʬ
void createZm();

//�������½�ʬ����
void updateZm();

//���������㶹�ӵ�
void createBullets();

//���������㶹�ӵ�����
void updateBullets();

//������⽩ʬ��ײ
void collisionCheck();

//�����㶹�ӵ��뽩ʬ����ײ���
void checkBullet2Zm();

//������ʬ��ֲ�����ײ���
void checkZm2ZhiWu();

int main() {
		
	gameInit();	//��Ϸ��ʼ��

	startUI();	//������Ϸ��ʼ����

	viewScence();	//����Ѳ��

	barsDown();	//״̬���»�

	//˫���壬�Ƚ�Ҫ���Ƶ�����һ���Ի�����ͼƬ�ϣ��ٰ�ͼƬ��������ⲻ�ϴ��ڴ��ȡ���ݶ����µ���Ļ��˸
	//��Ҫ��BeginBatchDraw()��EndBatchDraw()��FlushBatchDraw()���
	//BeginBatchDraw();		˫����ӵ�����Ῠ״̬��bug-���ڵļ�����ʱ�������

	int timer=0;
	bool flag = true;

	while (1)
	{
		userClick();	//��ȡ�û�����¼�

		timer += getDelay();	//��ȡ���ʱ��
		if (timer > 20) {	//��������ֲ����Ⱦʱ��
			timer = 0;
			flag = true;
		}
		if (flag) {

			flag = false;

			updateWindow();	//������Ϸ���ڣ���Ⱦ��

			updateGame();	//���¶���

			if (checkOver())break;	//�����Ϸ�Ƿ����
		}
		//ˢ��ͼƬ���������
		//FlushBatchDraw();
	}
	
	//EndBatchDraw();

	system("pause");

	return 0;
}

//��Ϸ��ʼ��ʵ��
void gameInit() {

	//�����������
	srand(time(NULL));

	//������Ϸ����ͼƬ
	loadimage(&imgBg, "res/bg.jpg");

	//����״̬��
	loadimage(&imgBar, "res/bar5.png");

	killZmCount = 0;
	zmCount = 0;
	gameStatus = GOING;

	memset(imgZhiWu, 0, sizeof(imgZhiWu));	//��ָ�븳��ֵ

	memset(map, 0, sizeof(map));	//��ʼ����ͼ����

	memset(balls, 0, sizeof(balls));	//��ʼ�������

	memset(zms, 0, sizeof(zms));	//��ʼ����ʬ��

	memset(bullets, 0, sizeof(bullets));	//��ʼ���㶹�ӵ���

	//����ֲ�￨��
	char	name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i+1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++) {	//20�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			//�ж��ļ��Ƿ����
			if (fileExist(name)) {
				imgZhiWu[i][j] = new IMAGE;
				loadimage(imgZhiWu[i][j], name);
			}
			else {
				break;
			}
		}		
	}

	//��ʼ��ѡ��ֲ��
	curZhiWu = 0;

	//��ʼ������ֵ
	sunShine = 50;

	//��������
	for (int i = 0; i < 29; i++) {	//29�ǹ̶�ֵ������Ѱ���ƥ��ķ�ʽ
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png",i+1);
		loadimage(&imgSunShineBall[i],name);
	}

	//���ؽ�ʬͼƬ
	for (int i = 0; i < 22; i++) {
		sprintf_s(name, sizeof(name), "res/zm/%d.png", i + 1);
		loadimage(&imgZm[i], name);
	}

	//���ؽ�ʬ����ͼƬ
	for (int i = 0; i < 20; i++) {
		sprintf_s(name, sizeof(name), "res/zm_dead/%d.png", i + 1);
		loadimage(&imgZmDead[i], name);  
	}

	//���ؽ�ʬ��ֲ��ͼƬ
	for (int i = 0; i < 21; i++) {
		sprintf_s(name, sizeof(name), "res/zm_eat/%d.png", i + 1);
		loadimage(&imgZmEat[i], name);
	}

	//����Ѳ����ʬͼƬ
	for (int i = 0; i < 11; i++) {
		sprintf_s(name, sizeof(name), "res/zm_stand/%d.png", i + 1);
		loadimage(&imgZmStand[i], name);
	}

	//�����㶹�ӵ�ͼƬ
	loadimage(&imgBulletNormal, "res/bullets/bullet_normal.png");
	
	//�����㶹�ӵ���ըͼƬ
	loadimage(&imgBulletBlast[3],"res/bullets/bullet_blast.png");
	for (int i = 0; i < 3; i++) {
		float	k = (i + 2) * 0.2;
		loadimage(&imgBulletBlast[i], "res/bullets/bullet_blast.png",
			imgBulletBlast[3].getwidth() * k, imgBulletBlast[3].getheight() * k, true);
	}

	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT,1/*,1*/);	//����1��ʾ�ٿ�һ������̨����	

	//��������
	LOGFONT	f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");	//��������Ч��
	f.lfQuality = ANTIALIASED_QUALITY;		//�����
	settextstyle(&f);
	setbkmode(TRANSPARENT);					//����ģʽ������͸��
	setcolor(BLACK);						//������ɫ����ɫ

}

//��Ϸ��ʼ����ʵ��
void startUI() {
	IMAGE imgMenu,imgMenu1,imgMenu2;
	int	flag = 0;
	loadimage(&imgMenu, "res/menu.png");	//���ؿ�ʼ����ͼ
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgMenu);	//��Ⱦ��ʼ����ͼ��������
		putimagePNG(474, 75, flag == 0 ? &imgMenu1 : &imgMenu2);

		ExMessage	msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_LBUTTONDOWN &&	//����������		��չ������꾭��ʱҲ���Ը���
				msg.x > 474 && msg.x < 774 && msg.y>75 && msg.y < 215) {
				flag = 1;
			}
		}
		else if (msg.message == WM_LBUTTONUP && flag == 1) {	//������̧��
			EndBatchDraw();
			return;
		}
		EndBatchDraw();
	}
}

//��������Ѳ��ʵ��
void viewScence() {
	int xMin = WIN_WIDTH - imgBg.getwidth();
	vector2 points[9] = { {550,80},{530,160},{630,170},{530,200},{525,270},		//9����ʬվλ
		{565,370},{605,340},{705,280},{690,340}};
	int index[9];
	for (int i = 0; i < 9; i++) {
		index[i] = rand() % 11;
	}
	int count = 0;
	for (int x = 0; x >= xMin; x -= 4) {	//ÿ֡�ƶ�2����
		BeginBatchDraw();

		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;

		EndBatchDraw();
		Sleep(5);
	}

	//ͣ��1s
	for (int i = 0; i < 100; i++) {
		BeginBatchDraw();

		putimage(xMin, 0, &imgBg);
		for (int j = 0; j < 9; j++) {
			putimagePNG(points[j].x, points[j].y, &imgZmStand[index[j]]);
			index[j] = (index[j] + 1) % 11;
		}

		EndBatchDraw();
		Sleep(20);
	}


	//������
	for (int x = xMin; x <= -112; x += 4) {	//ÿ֡�ƶ�2����
		BeginBatchDraw();

		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++) {
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZmStand[index[k]]);
			if (count >= 10) {
				index[k] = (index[k] + 1) % 11;
			}
			if (count >= 10)count = 0;
		}
		
		EndBatchDraw();
		Sleep(5);
	}
}

//״̬���»�ʵ��
void barsDown() {
	int height = imgBar.getheight();
	for (int y = -height; y <= 0; y++) {
		BeginBatchDraw();

		putimage(-112, 0, &imgBg);

		putimagePNG(250, y, &imgBar);

		for (int i = 0; i < ZHI_WU_COUNT; i++) {
			int x = 338 + i * 65;
			putimagePNG(x, 6 + y, &imgCards[i]);
		}

		EndBatchDraw();
		Sleep(5);
	}
}

//��ͼƬ���ص�������(��Ⱦ)
void updateWindow() {

	BeginBatchDraw();

	putimage(-112, 0, &imgBg);	//����(��Ⱦ)������

	putimagePNG(255, 0, &imgBar);	//����(��Ⱦ)״̬��

	for (int i = 0; i < ZHI_WU_COUNT; i++) {	//����(��Ⱦ)ֲ�￨��
		int x = 343 + i * 65;
		int y = 6;
		putimagePNG(x, y, &imgCards[i]);
	}

	//�ڵ�ͼ�ϼ���(��Ⱦ)ֲ��
	for (int i = 0; i < 3; i++) {	
		for (int j = 0; j < 9; j++){
			if (map[i][j].type > 0) {
				//int x = 256 + j * 81;
				//int y = 179 + i * 102 + 14;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				putimagePNG(map[i][j].x, map[i][j].y, imgZhiWu[zhiWuType][index]);
			}
		}
	}

	//����(��Ⱦ)�϶���ֲ��
	if (curZhiWu > 0) {
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];	
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	//for (int i = 0; i < ballMax; i++) {	//��ʱ��Ⱦ�ᱻ��ʬ��ס
	//	if (balls[i].used || balls[i].xoff) {	//����(��Ⱦ)����
	//		IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
	//		putimagePNG(balls[i].x, balls[i].y, img);
	//	}
	//}

	//����(��Ⱦ)����ֵ
	char scoreText[8];
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunShine);	//������ֵת�����ַ�����
	outtextxy(283, 67, scoreText);			//��Ⱦ���	λ�ÿɵ����ɾ���,����ʹ�ù̶�ֵy	

	//����(��Ⱦ)��ʬ
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used) {	
			//IMAGE* img = &imgZm[zms[i].frameIndex];
			//IMAGE* img = (zms[i].dead) ? imgZmDead : imgZm;
			
			IMAGE* img = NULL;
			if (zms[i].eating) {
				img = imgZmEat;
			}else if(zms[i].dead) {
				img = imgZmDead;
			}
			else {
				img = imgZm;
			}

			img += zms[i].frameIndex;
			putimagePNG(zms[i].x, zms[i].y-img->getheight(), img);
		}
	}

	//����(��Ⱦ)�㶹�ӵ�
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			if (bullets[i].blast) {	//�㶹�ӵ���ײ��Ⱦ
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];
				//IMAGE* img = &imgBulletBlast[];
				putimagePNG(bullets[i].x, bullets[i].y, img);
				FlushBatchDraw();
			}
			else {	//�㶹�ӵ���ͨ��̬��Ⱦ
				putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal);
			}
		}
	}

	//����(��Ⱦ)����
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used /* || balls[i].xoff*/) {
			IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}

	EndBatchDraw();
}

//�û����ʵ��
void userClick() {
	static	int status = 0;
	ExMessage	msg;
	if (peekmessage(&msg)) {	//�ж��û��Ƿ��в���
		if (msg.message == WM_LBUTTONDOWN) {	//����������
			if (msg.x > 343 && msg.x < 343 + 65 * ZHI_WU_COUNT && msg.y < 96) {	//������Ƶ��¼�
				int index = (msg.x - 343) / 65;
				
				//�ж�����ֵ�Ƿ��㹻����ֲ��
				if (index == XIANG_RI_KUI) {
					if (sunShine >= 50) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 50;
					}
				}
				else if (index == WAN_DAO) {
					if (sunShine >= 100) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 100;
					}
				}
				else if (index == SHI_REN_HUA) {
					if (sunShine >= 150) {
						status = 1;
						curZhiWu = index + 1;
						//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
						curX = msg.x;
						curY = msg.y;
						sunShine -= 150;
					}
				}
				//�����Ļ��д�����

				//status = 1;
				//ʹֲ����ʾ�ڵ��λ�ã�������ֲ��������ϴ���ʧλ�õ�Сbug
				//curX = msg.x;
				//curY = msg.y;
			}
			else {	//�ռ������¼�
				collectSunshine(&msg);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1) {	//����ƶ�
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_RBUTTONDOWN && status == 1) {	//����Ҽ�����
			if (msg.x > 256 - 112 && msg.x < 900 - 30 && msg.y > 179 && msg.y < 489) {
				int	row = (msg.y - 179) / 102;	//��ȡ��
				int	col = (msg.x - 256 + 112) / 81;	//��ȡ��
				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;	//����굱ǰ������ֲ��
					map[row][col].frameIndex = 0;	//��Ⱦֲ���һ֡
					map[row][col].shootTimer = 0;	//��ʼ������ʱ��

					map[row][col].x = 256 - 112 + col * 81;	//ֲ������
					map[row][col].y = 179 + row * 102 + 14;
				}
			}
			//ʹֲ���ͷ���ʧ
			curZhiWu = 0;
			status = 0;
			//����ֲ�������
			//curX = 1000;
			//curY = 1000;
		}
	}
}

//�ж��ļ��Ƿ����ʵ��
bool fileExist(const char* name) {
	FILE* fp=fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

//���¶���ʵ��
void updateGame() {

	//����ֲ�ﶯ��
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int	zhiWuType = map[i][j].type - 1;
				int	index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}

	//������ʬ
	createZm();

	//���½�ʬ����
	updateZm();
	
	//��������
	createSunShine();

	//�������⶯��
	updateSunShine();

	//�����㶹�ӵ�
	createBullets();	//�㶹����֮��Ƶ�ʲ�һ��

	//�����㶹�ӵ�����
	updateBullets();

	//�㶹�ӵ��뽩ʬ��ײ
	collisionCheck();
}

//�����Ϸ�Ƿ����ʵ��
bool checkOver() {
	BeginBatchDraw();

	bool ret = false;

	if (gameStatus == WIN) {
		Sleep(100);
		loadimage(0, "res/gameWin.png");
		mciSendString("play res/win.mp3", 0, 0, 0);
		ret = true;
	}
	else if (gameStatus == FAIL) {
		Sleep(100);
		loadimage(0, "res/gameFail.png");
		mciSendString("play res/lose.mp3", 0, 0, 0);
		ret = true;
	}
	EndBatchDraw();
	return ret;
}

//��������ʵ��
void createSunShine() {

	static	int	count = 0;
	static	int	fre = 200;
	count++;
	
	if (count >= fre) {	//�����������ɵ��ٶ�

		fre = 100 + rand() % 150;	//�ڶ������������ʱ�����
		count = 0;

		int i;

		//���������ȡ�����õ�����
		for (i = 0; i < ballMax && balls[i].used; i++);	//���ʣ��ʾ���һ���¶��巽ʽ����{}һ����
		if (i >= ballMax)return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		//balls[i].x = 260 + rand() % (900 - 320);	//������
		//balls[i].y = 60;
		//balls[i].destY = 200 + (rand() % 4) * 90;	//���ֹͣλ��
		//balls[i].xoff = 0;
		//balls[i].yoff = 0;
		balls[i].timer = 0;

		//�Ż�
		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		balls[i].p1 = vector2(260 - 112 + rand() % (900 - 320 + 112), 60);
		balls[i].p4 = vector2(balls[i].p1.x, 200 + (rand() % 4) * 90);
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);

	}

	//���տ���������
	for (int i = 0; i < 3; i++) {
		for(int j = 0; j < 9; j++) {
			if (map[i][j].type == XIANG_RI_KUI + 1) {
				map[i][j].timer++;
				if (map[i][j].timer > 200) {
					map[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);
					if (k >= ballMax)return;

					balls[k].used = true;

					balls[k].p1 = vector2(map[i][j].x, map[i][j].y);	//���ñ��������ߵĲ���
					int w = (50 + rand() % 51) * (rand() % 2 ? 1 : -1);
					balls[k].p4 = vector2(map[i][j].x + w, map[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight()
						- imgSunShineBall->getheight());
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 100);
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 150);

					balls[k].status = SUNSHINE_PRODUCT;
					balls[k].speed = 0.05;
					balls[k].t = 0;
				}
			}
		}
	}
}

//�������⶯��ʵ��
void updateSunShine() {
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;	//��������֡

			if (balls[i].status == SUNSHINE_DOWN) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t >= 1) {
					sun->status = SUNSHINE_GROUND;
					sun->t = 0;
					sun->timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND) {
				balls[i].timer++;
				if (balls[i].timer > 100) {
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1) {
					sunShine += 25;
					sun->used = false;
					sun->t = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT) {
				struct sunShineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);
				if (sun->t > 1) {
					sun->status = SUNSHINE_GROUND;
					sun->t = 0;
					sun->timer = 0;
				}
			}

			/*if (balls[i].y < balls[i].destY) {
				balls[i].y += 2;
			}
			if (balls[i].y >= balls[i].destY) {
				balls[i].timer++;
				//cout << "i=" << i << ":" << balls[i].timer << endl;
				if (balls[i].timer > 100) {
					balls[i].timer = 0;	//���ö�ʱ����������һ��ȡ��ͬ����������һ����ֹͣλ�þ���ʧ
					balls[i].used = false;
					//printf((balls[i].timer));
					//cout << "i=" << i << ":" << balls[i].timer << endl;
				}
			}*/
		}
		/*else if (balls[i].xoff) {	//�ƶ�������
			//�����������ƫ��ֵ
			float	destY = 0;
			float	destX = 262;

			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//���Ǻ���

			balls[i].xoff = 4 * cos(angle);
			balls[i].yoff = 4 * sin(angle);

			balls[i].x -= balls[i].xoff;
			balls[i].y -= balls[i].yoff;

			if (balls[i].x < 262 || balls[i].y < 0) {
				//sunShine += 25;	//�����������ֵ���bug	
				balls[i].xoff = 0;
				balls[i].yoff = 0;
				
			}
		}*/
	}
}

//�ռ�����ʵ��
void collectSunshine(ExMessage* msg) {
	int w = imgSunShineBall[0].getwidth();	//����������Ŀ��
	int h = imgSunShineBall[0].getheight();	//����������ĸ߶�
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {	//������ʹ���˲Ž��в���
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;
			if (msg->x > x && msg->x<x + w &&		//ֻ�е���������ⷶΧ�ڲŽ��в���
				msg->y>y && msg->y < y + h) {
				
				//balls[i].used = false;	//��������ʧ
				
				balls[i].status = SUNSHINE_COLLECT;

				//sunShine += 25;			//����ֵ��25
				
				mciSendString("play res/sunshine.mp3", 0, 0, 0);	

				//PlaySound("res/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);	//����������û��Ч��Ҫʹ��WAV��ʽ������

				//�����������ƫ��ֵ
				//float	destY = 0;
				//float	destX = 262;

				//float angle = atan((balls[i].y - destY) / (balls[i].x - destX));	//���Ǻ���

				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);

				//�Ż�
				balls[i].p1 = balls[i].pCur;
				balls[i].p4 = vector2(262, 0);
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);
				float off = 8;
				balls[i].speed = 1.0 / (distance / off);
				break;
			}
		}
	}
}

//������ʬʵ��
void createZm() {

	if (zmCount >= ZM_MAX) {
		return;
	}

	static	int	count = 0;
	static	int	zmFre = 500;
	count++;

	if (count >= zmFre) {	//���ƽ�ʬ���ɵ��ٶ�

		zmFre = 300 + rand() % 200;	//�ڶ������ɽ�ʬ��ʱ�����
		count = 0;

		int i;

		//�ӽ�ʬ����ȡ�����õĽ�ʬ
		for (i = 0; i < zmMax && zms[i].used; i++);	//���ʣ��ʾ���һ���¶��巽ʽ����{}һ����������&&!xx
		if (i >= zmMax)return;

		zms[i].used = true;
		//zms[i].frameIndex = 0;
		zms[i].x = WIN_WIDTH;
		zms[i].row = rand() % 3;
		zms[i].y = 172 + (zms[i].row + 1) * 100;
		zms[i].speed = 1;
		zms[i].blood = 100;
		zms[i].dead = false;
		zms[i].eating = false;

		zmCount++;
	}
}

//���½�ʬ����ʵ��
void updateZm() {
	//���½�ʬλ��
	static int count = 0;
	count++;
	if (count > 2) {	//���ƽ�ʬǰ���ٶ�
		count = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 48) {
					//������Ϸ
					gameStatus = FAIL;
				}
			}
		}
	}
	
	//���½�ʬ����
	static int count2=0;
	count2++;
	if (count2 > 4) {	//���ƽ�ʬ���������ٶȣ��������
		count2 = 0;
		for (int i = 0; i < zmMax; i++) {
			if (zms[i].used) {
				if (zms[i].dead) {
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20) {
						zms[i].used = false;
						killZmCount++;
						if (killZmCount == ZM_MAX) {
							gameStatus = WIN;
						}
					}
				}
				else if (zms[i].eating) {
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 20;
				}
				else
				{
					zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;	//��ʬ������	-δ���(�������⣬�������10�ͻ����֣�������)
					//cout << "i=" << i << zms[i].frameIndex << endl;	//���ư������ؽ�ʬͼƬʱ���������ش���
				}
			}
		}
	}
}

//�����㶹�ӵ�ʵ��
void createBullets() {
	int lines[3] = { 0 };
	int	dangerX = WIN_WIDTH - imgZm[0].getwidth() + 50;	//���忪ʼ�������
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].used && zms[i].x < dangerX) {
			lines[zms[i].row] = 1;	//��ʬ�����иı��
			//cout <<"��ʬ������" << zms[i].row << endl;
		}
	}
	for (int i = 0; i < 3; i++) {	//3��9�̶�ֵ����չ�Բ���ǽ���궨��
		for (int j = 0; j < 9; j++) {
			if (lines[i] && map[i][j].type == 1) {	//���㶹�ҽ�ʬ�ߵ������Χ
				map[i][j].shootTimer++;
				if (map[i][j].shootTimer > 20) {
					map[i][j].shootTimer = 0;
					int k;
					for (k = 0; k < bulletMax && bullets[k].used; k++);
					if (k >= bulletMax) return;
						bullets[k].used = true;		//��ʼ���㶹�ӵ�
						bullets[k].row = i;
						//cout << "i=" << i << endl;
						bullets[k].speed = 4;
						bullets[k].blast = false;
						bullets[k].frameIndex = 0;

						int zwX = 256 - 112 + j * 81;
						int zwY = 179 + i * 102 + 14;
						bullets[k].x = zwX + imgZhiWu[0][0]->getwidth() - 10;
						bullets[k].y = zwY + 5;
						lines[i] = 0;
				}
			}
		}
	}
}

//�����㶹�ӵ�����ʵ��
void updateBullets() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used) {
			bullets[i].x += bullets[i].speed;
			if (bullets[i].x > WIN_WIDTH) {
				bullets[i].used = false;
			}
			//��ײ���������ʧ
			if (bullets[i].blast) {
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3) {	//����3�ᵼ��״̬����˸
					bullets[i].used = false;
				}
			}
		}
	}
}

//��齩ʬ��ײʵ��
void collisionCheck() {

	//�㶹�ӵ��뽩ʬ����ײ���
	checkBullet2Zm();

	//��ʬ��ֲ�����ײ���
	checkZm2ZhiWu();
}

//�㶹�ӵ��뽩ʬ����ײ���ʵ��
void checkBullet2Zm() {
	for (int i = 0; i < bulletMax; i++) {
		if (bullets[i].used == false || bullets[i].blast)continue;	//����㶹�ӵ�ûʹ�û����Ѿ���ʼ��ײ��������
		for (int j = 0; j < zmMax; j++) {
			if (zms[j].used == false)continue;
			int x1 = zms[j].x + 80;
			int x2 = zms[j].x + 110;
			int x = bullets[i].x;
			if (zms[j].dead == false && bullets[i].row == zms[j].row && x > x1 && x < x2) {	//�㶹�ӵ��뽩ʬ��ײ��
				zms[j].blood -= 10;//5;//10;//20;
				bullets[i].blast = true;
				bullets[i].speed = 0;

				if (zms[j].blood <= 0) {
					zms[j].dead = true;
					zms[j].speed = 0;
					zms[j].frameIndex = 0;
				}
				break;
			}
		}
	}
}

//��ʬ��ֲ�����ײ���ʵ��
void checkZm2ZhiWu() {
	for (int i = 0; i < zmMax; i++) {
		if (zms[i].dead)continue;
		int row = zms[i].row;
		for (int k = 0; k < 9; k++) {
			if (map[row][k].type == 0)continue;
			int zhiWuX = 256 - 112 + k * 81;	//���彩ʬ���Է�Χ
			int x1 = zhiWuX + 10;
			int x2 = zhiWuX + 60;
			int x3 = zms[i].x + 80;

			if (x3 > x1 && x3 < x2) {
				if (map[row][k].catched==true){	//��ʬ�ԵĹ����е�һЩ����
					map[row][k].deadTimer++;
					if (map[row][k].deadTimer > 100) {	//��ʬ������-���ò���
						map[row][k].deadTimer = 0;
						map[row][k].type = 0;
						zms[i].eating = false;
						zms[i].frameIndex = 0;
						zms[i].speed = 1;
					}
				}
				else {	//��ʬ����-���ò���
					map[row][k].catched = true;
					map[row][k].deadTimer = 0;
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
				}
			}
		}
	}
}
