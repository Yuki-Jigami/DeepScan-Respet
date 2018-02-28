//
// DeepScan respect
//
// VisualStudioでもコンパイルできるようにするため
// ソースコードをUTF-8(BOM付き)で用意:D
//

// Frameworkの全てをインクルード
#include "lib/framework.hpp"
#include <ctime>

#define DCMAX 8
#define ENEMY_MAX 13

struct _screen {
	bool title = true;
	bool game = false;
	bool hit = false;
	bool gameover = false;
}screen;

struct _player {
	int x = -140;
	float y = 300.0f;
	int vx = 3;
	int score = 000000;
	int dc_left = 8;
}player;

struct _depthcharge {
	int usage = 0;
	float x, y;
	float a = 0.0f;
	bool hit = false;
}depthcharge[DCMAX];

struct _enemy {
	float x;
	float y;
	float vx = 0.0f;
	int LR;
	int alive = 0;
	int point = 0;
	int mine = 0;
	bool kill = false;
	Texture texture;
}enemy[ENEMY_MAX];

struct _enemyrandom {
	float y, y2;     //深度のランダム範囲
	float vx, vx2;   //速度のランダム範囲
}enemyrandom[9] = {
		{200, 250, 1.2f, 1.7f},
		{150, 220, 1.5f, 1.8f},
		{100, 170, 1.7f, 2.2f},
		{20, 100, 2.1f, 2.5f},
		{-80, 0, 2.4f, 2.8f},
		{-120,-85, 2.8f, 3.5f},
		{-170, -125, 3.3f, 3.8f},
		{-245, -175, 3.7f, 4.2f},
		{-289,-250, 4.3f, 5.0f}
};

struct _Effects {
	bool active;
	float x, y;
	int frame_count;
	int alive = 0;
	int index;
};

_Effects enemy_explode[ENEMY_MAX];
_Effects mine_explode[ENEMY_MAX];

struct _seamine {
	float x;
	float y;
	float vy = 1.0f;
	int alive = 0;
	int explodecount = 0;
	bool explode = false;
}seamine[ENEMY_MAX];

struct _keyinputs {
	bool left = false;
	bool right = false;
	bool dcL = false;
	bool dcR = false;
	bool space = false;
	bool enter = false;
}keyinputs;

void dcshokika(int i) {
	if (depthcharge[i].usage == 0) {
		depthcharge[i].x = -800;
		depthcharge[i].y = -800;
		depthcharge[i].a = 0;
	}
}

void eneshokika(int i) {
	if (enemy[i].alive == 0) {
		enemy[i].x = 800;
		enemy[i].y = 800;
		enemy[i].vx = 0;
		enemy[i].point = 0;
		enemy[i].mine = 0;
	}
}

void mineshokika(int i) {
	if (seamine[i].alive == 0) {
		if (seamine[i].explode == false) {
			seamine[i].x = 0;
			seamine[i].y = 0;
			seamine[i].explodecount = 0;
		}
	}
}

void effectshokika(_Effects effect[], int i) {
	if (effect[i].active == false) {
		effect[i].x = 800;
		effect[i].y = 800;
		effect[i].frame_count = 0;
		effect[i].index = 0;
	}
}

void initiateall(void) {
	//player
	player.x = -140;
	player.y = 300.0f;
	player.score = 000000;
	//enemy
	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy[i].x = 800;
		enemy[i].y = 800;
		enemy[i].vx = 0;
		enemy[i].alive = 0;
		enemy[i].point = 0;
		enemy[i].LR = 0;
		enemy[i].mine = 0;
		enemy[i].kill = false;
		//mine
		seamine[i].x = 800;
		seamine[i].y = 800;
		seamine[i].alive = 0;
		seamine[i].explodecount = 0;
		seamine[i].explode = false;
		//effects
		enemy_explode[i].x = 800;
		enemy_explode[i].y = 800;
		enemy_explode[i].frame_count = 0;
		enemy_explode[i].active = false;
		enemy_explode[i].index = 0;
		mine_explode[i].x = 800;
		mine_explode[i].y = 800;
		mine_explode[i].frame_count = 0;
		mine_explode[i].active = false;
		mine_explode[i].index = 0;
	}
	//dc
	for (int i = 0; i < DCMAX; i++) {
		depthcharge[i].x = -800;
		depthcharge[i].y = -800;
		depthcharge[i].usage = 0;
		depthcharge[i].hit = false;
	}
}

void dcCreat(int& dx, float& y, Media fire) {
	int i = 0;
	int no = -1;

	//-----------爆雷生成-------------
	for (i = 0; i < DCMAX; i++) {
		if (depthcharge[i].usage == 0) {
			dcshokika(i);
			no = i;
			break;
		}
	}

	if (no == -1) {
		return;
	}
	depthcharge[no].usage = 1;
	depthcharge[no].x = dx;
	depthcharge[no].y = y;
	depthcharge[no].a += 0.05;
	fire.play();

}

void dclaunch(float x, float& y, Media fire) {//爆雷の発射
	int dx = 0;
	if (keyinputs.dcL == true) {
		dx = x - 6;
		keyinputs.dcL = false;
	}
	if (keyinputs.dcR == true) {
		dx = x + 86;
		keyinputs.dcR = false;
	}
	dcCreat(dx, y, fire);
}

void enemy_creat(int& point, float& y, float& vx, int& LR, Texture null) {
	int no = -1;
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemy[i].alive == 0) {
			eneshokika(i);
			no = i;
			break;
		}
	}
	if (no == -1) {
		return;
	}
	enemy[no].alive = 1;
	enemy[no].point = point;  //dice(1, 9);
	enemy[no].y = y;  //dice(enemyrandom[epoint].y, enemyrandom[epoint].y2);
	enemy[no].vx = vx;  //dice(enemyrandom[epoint].vx, enemyrandom[epoint].vx2);
	enemy[no].LR = LR;  //dice(1, 2);
	enemy[no].texture = null;
	if (LR == 1) {
		enemy[no].x = -525;
	}
	else if (LR == 2) {
		enemy[no].x = 320;
		enemy[no].vx = enemy[no].vx * -1;
	}
}

void enemy_launch(Texture hairetu[]) {
	Random dice;
	dice.setSeed(std::time(nullptr));
	Texture enemy_texture;
	int pp;
	int point;
	float y;
	float vx;
	int LR;
	point = dice(1, 9);
	pp = point - 1;
	y = dice(enemyrandom[pp].y, enemyrandom[pp].y2);
	vx = dice(enemyrandom[pp].vx, enemyrandom[pp].vx2);
	LR = dice(1, 2);
	enemy_texture = hairetu[pp];
	enemy_creat(point, y, vx, LR, enemy_texture);
}

void mine_creat(float& x, float& y) {
	int no = -1;
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (seamine[i].alive == 1) {
			continue;
		}
		if (enemy[i].mine == 0) {
			mineshokika(i);
			no = i;
			break;
		}
	}
	if (no == -1) {
		return;
	}
	enemy[no].mine = 1;
	seamine[no].alive = 1;
	seamine[no].x = x;
	seamine[no].y = y;
}

int dcinenemy(float dcx, float dcy, int dcw, int dch, float enemy_x, float enemy_y, float enemy_w, float enemy_h) {
	if (enemy_y < dcy
		&& (dcy + dch) < (enemy_y + enemy_h)
		&& (enemy_x - 14) < dcx
		&& (dcx + dcw) < (enemy_x + enemy_w)) {
		return 1;
	}
	else {
		return 0;
	}
}

int playerinmine(int player_x, int player_w, float mine_x, float mine_w) {
	if (player_x < mine_x
		&& (mine_x + mine_w) < (player_x + player_w)) {
		return 1;
	}
	else {
		return 0;
	}
}

void dchit(Media bakuhatu) {
	//爆雷の当たり判定やり直し
	for (int j = 0; j < ENEMY_MAX; j++) {
		if (enemy[j].alive == 0) {
			continue;
		}
		for (int i = 0; i < DCMAX; i++) {
			if (depthcharge[i].usage == 0) {
				continue;
			}
			if (dcinenemy(depthcharge[i].x, depthcharge[i].y, 5, 11, enemy[j].x, enemy[j].y, 63, 28) == 1) {
				enemy[j].kill = true;
				depthcharge[i].hit = true;
				bakuhatu.play();
			}
		}
	}

}

void effectinitiate(_Effects effect[], float& x, float& y, int w, int h) {
	for (int k = 0; k < ENEMY_MAX; k++) {
		if (effect[k].active == true)
			continue;

		if (effect[k].active == false) {
			effect[k].active = true;
			effect[k].alive = 1;
			effect[k].x = x - w;
			effect[k].y = y + h;
			effect[k].frame_count = 0;
			break;
		}
	}

}

void Movement(Media bakuhatu,Media bushaaa) {
	dchit(bakuhatu);
	//----------爆雷の移動------------
	for (int i = 0; i < DCMAX; i++) {
		if (depthcharge[i].usage == 0)
			continue;

		if (depthcharge[i].usage == 1) {
			depthcharge[i].y -= 0.8;
		}
		if (depthcharge[i].y <= -295) {
			depthcharge[i].usage = 0;
		}
		depthcharge[i].a += 0.05;

		if (depthcharge[i].hit == true) {
			depthcharge[i].usage = 0;
			depthcharge[i].hit = false;
			break;
		}
	}
	//----------敵の移動-------------
	for (int j = 0; j < ENEMY_MAX; j++) {
		if (enemy[j].alive == 0)
			continue;

		if (enemy[j].alive == 1) {
			enemy[j].x += enemy[j].vx;
		}
		if (enemy[j].x < -550 || enemy[j].x > 350) {
			enemy[j].alive = 0;
			enemy[j].mine = 0;
		}

	}
	//-------機雷の移動------------
	for (int k = 0; k < ENEMY_MAX; k++) {
		if (seamine[k].alive == 0) {
			continue;
		}

		if (seamine[k].alive == 1) {
			seamine[k].y += seamine[k].vy;
		}
		if (seamine[k].y >= 298) {
			seamine[k].explode = true;
			bushaaa.play();
		}


	}
}

void draw(Texture bottom,Texture abababab, Texture self) {//描画

	//-------------背景や枠---------------
	drawFillBox(-450, -500, 700, 800, Color(0, 0, 1));//海
	drawFillBox(-450, 300, 700, 200, Color(0.4, 0.4, 1));//空
	drawTextureBox(-447, -296, 695, 30, 0, 0, 512 * 8, 256, bottom);//海底

	//---------------自機-------------

	//drawFillBox(player.x, 300, 80, 30, Color(0, 1, 0));
	drawTextureBox(player.x - 20, 270, 120, 60, 0, 256, 1024, 512, self);

	//---------------爆雷-------------
	for (int i = 0; i < DCMAX; i++) {
		if (depthcharge[i].usage == 1) {
			drawFillBox(depthcharge[i].x, depthcharge[i].y, 5, 11, Color(1, 0, 0),
				depthcharge[i].a, Vec2f(1, 1), Vec2f(2, 5));
		}
	}

	//--------------敵----------------
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (enemy[i].alive == 1) {
			//drawFillBox(enemy[i].x, enemy[i].y, 56, 28, Color(0, 1, 0));
			drawTextureBox(enemy[i].x, enemy[i].y - 2, 56, 32, 0, 0, 1024, 512, enemy[i].texture);
		}
	}

	//------------機雷---------------
	for (int i = 0; i < ENEMY_MAX; i++) {
		if (seamine[i].alive == 1) {
			drawFillBox(seamine[i].x, seamine[i].y, 5, 8, Color(1, 1, 1));
		}
	}

	//-----------右側の枠とか外枠とか
	drawFillBox(250, -500, 300, 1000, Color(0, 0, 0));//右側の黒背景
	drawFillBox(-750, -500, 300, 1000, Color(0, 0, 0));//全画面の時用に左側の黒背景
	drawLine(250, -500, 250, 1000, 4, Color(1, 1, 1));//右側の枠線
	drawLine(-450, -300, 250, -300, 7, Color(1, 1, 1));//下の線
	drawBox(-450, -500, 900, 1000, 5, Color(1, 1, 1));//外枠
	drawTextureBox(250, -400, 200, 400, 0, 0, 1024, 2048, abababab);//説明
}

enum Size {
	WIDTH = 900,
	HEIGHT = 1000
};

//
// メインプログラム
//
int main() {
	// アプリウインドウを生成
	// GLFWとOpenGLの初期化
	// TIPS:色々な型が依存しているので、最初に生成する事
	AppEnv env(Size::WIDTH, Size::HEIGHT);

	Texture bottom("res/sozai/seabottom.png");
	Texture explosion("res/sozai/explosion1.png");
	Texture mineexplosion("res/sozai/mineexplosion.png");
	Texture titleback("res/sozai/title.png");
	Texture gameoverback("res/sozai/gameover.png");
	Texture instruction("res/sozai/instruction.png");
	Texture dc("res/sozai/dc.png");
	Texture ship("res/sozai/ship.png");
	Texture one("res/sozai/submarine1.png");
	Texture two("res/sozai/submarine2.png");
	Texture three("res/sozai/submarine3.png");
	Texture four("res/sozai/submarine4.png");
	Texture five("res/sozai/submarine5.png");
	Texture six("res/sozai/submarine6.png");
	Texture seven("res/sozai/submarine7.png");
	Texture eight("res/sozai/submarine8.png");
	Texture nine("res/sozai/submarine9.png");
	Texture hairetu[9] = {
		one,two,three,four,five,six,seven,eight,nine
	};
	

	//Media huwahuwa("res/sound/huwahuwa.wav");
	Media sonar("res/sound/nc22612.wav");
	Media bakuhatu("res/sound/se_maoudamashii_retro23.wav");
	Media bushaaa("res/sound/se_maoudamashii_retro28.wav");
	Media bakuhatushisan("res/sound/se_maoudamashii_retro30.wav");
	Media fire("res/sound/nc79080.wav");
	Media system("res/sound/se_maoudamashii_system03.wav");
	Media piano("res/sound/08 ピアノソナタ第7番「憂愁」.wav");

	//huwahuwa.looping(true);
	sonar.looping(true);
	piano.looping(true);
	piano.gain(1.0);

	Font font("res/03スマートフォントUI.otf");
	font.size(30);

	Random random;
	float y = 300;
	float dcy = 300;

	int enemy_delay = 0;

	for (int i = 0; i < ENEMY_MAX; i++) {
		enemy_explode[i].active = false;
		mine_explode[i].active = false;
	}

	while (env.isOpen()) {

		env.begin();


	title:
		env.flushInput();
		if (screen.title == true) {
			//huwahuwa.play();
			piano.stop();
		}

		while (screen.title) {
			//タイトル画面
			env.begin();
			initiateall();
			screen.hit = false;
			drawTextureBox(-1000, -500, 2000, 1000, 0, 0, 1024, 512, titleback);

			if (env.isKeyPushed(' ')) {
				system.play();
				screen.title = false;
				screen.game = true;
				goto game;
				break;
			}
			env.end();

		}
		env.flushInput();



	game:
		env.flushInput();
		if (screen.game == true) {
			//huwahuwa.stop();
			piano.stop();
			sonar.play();
		}
		while (screen.game) {
			//ゲーム中
			env.begin();
			screen.hit = false;
			Movement(bakuhatu,bushaaa);
			//enemy
			enemy_delay++;
			enemy_delay %= 120;

			if (!enemy_delay)
				enemy_launch(hairetu);

			float minerandom = 0.05f;
			if (random() < 0.005f) {
				for (int i = 0; i < ENEMY_MAX; i++) {
					if (enemy[i].alive == 0 || seamine[i].alive == 1 || enemy[i].mine >= 1) {
						continue;
					}
					if (enemy[i].x > -440 && enemy[i].x < 240) {
						mine_creat(enemy[i].x, enemy[i].y);
						break;
					}
				}
			}
			for (int k = 0; k < ENEMY_MAX; k++) {
				if (seamine[k].explode == true) {
					seamine[k].alive = 0;
					seamine[k].explodecount++;
					if (seamine[k].explodecount < 60) {
						if (playerinmine(player.x, 80, seamine[k].x, 5) == 1) {
							screen.hit = true;
							break;
						}
					}
					else if (seamine[k].explodecount >= 60) {
						player.score = player.score + 1;
						seamine[k].explode = false;
						break;
					}
				}
			}

			if (screen.hit == true) {
				bakuhatushisan.play();
				screen.game = false;
				screen.gameover = true;
				screen.hit = false;
				goto gameover;
				break;
			}

			//-----------------爆雷の発射----------------------------------
			if (env.isKeyPushed(KEY_LEFT) || env.isKeyPushed(KEY_RIGHT)) {
				if (env.isKeyPushed(KEY_LEFT))
					keyinputs.dcL = true;
				if (env.isKeyPushed(KEY_RIGHT))
					keyinputs.dcR = true;

				dclaunch(player.x, dcy, fire);
			}

			//------------------自機の移動------------------------------

			if (env.isKeyPressing('A'))
				player.x -= player.vx;
			if (env.isKeyPressing('D'))
				player.x += player.vx;
			if (player.x <= -448) {
				player.x = -448;
			}
			if (player.x >= 168) {
				player.x = 168;
			}

			//-----------描画---------------
				draw(bottom, instruction, ship);
			for (int i = 0; i < ENEMY_MAX; i++) {
				if (seamine[i].explode == false)
					continue;

				if (seamine[i].explode == true) {
					effectinitiate(mine_explode, seamine[i].x, player.y, 20, -15);
				}
			}
			for (int j = 0; j < ENEMY_MAX; j++) {
				if (mine_explode[j].active != true) continue;

				mine_explode[j].frame_count += 1;
				mine_explode[j].index = mine_explode[j].frame_count / 2;
				if (mine_explode[j].index < 30) {
					if (mine_explode[j].alive == 1) {
						float sx = (mine_explode[j].index % 8) * 256;
						float sy = (mine_explode[j].index / 8) * 256;
						drawTextureBox(mine_explode[j].x, mine_explode[j].y, 40, 40, sx, sy, 256, 256, mineexplosion);
					}
				}
				if (mine_explode[j].index >= 30) {
					mine_explode[j].active = false;
					mine_explode[j].alive = 0;
					effectshokika(mine_explode, j);
					break;
				}
			}
			for (int i = 0; i < ENEMY_MAX; i++) {
				if (enemy[i].kill == false)
					continue;

				if (enemy[i].kill == true) {
					effectinitiate(enemy_explode, enemy[i].x, enemy[i].y, 26, -20);
					enemy[i].alive = 0;
					enemy[i].mine = 0;
					player.score = player.score + enemy[i].point;
					enemy[i].kill = false;
				}
			}
			for (int j = 0; j < ENEMY_MAX; j++) {
				if (enemy_explode[j].active != true) continue;

				enemy_explode[j].frame_count += 1;
				enemy_explode[j].index = enemy_explode[j].frame_count / 2;
				if (enemy_explode[j].index < 30) {
					if (enemy_explode[j].alive == 1) {
						float sx = (enemy_explode[j].index % 8) * 256;
						float sy = (enemy_explode[j].index / 8) * 256;
						drawTextureBox(enemy_explode[j].x, enemy_explode[j].y, 120, 60, sx, sy, 256, 256, explosion);
					}
				}
				if (enemy_explode[j].index >= 30) {
					enemy_explode[j].active = false;
					enemy_explode[j].alive = 0;
					effectshokika(enemy_explode, j);
					break;
				}
			}
			for (int i = 0; i < DCMAX; i++) {
				if (depthcharge[i].usage == 1)continue;

				if (depthcharge[i].usage == 0) {
					int x = i * 25;
					drawTextureBox(x-15, 450, 30, 30, 0, 0, 512, 512, dc);
				}
			}
			//---------新要素ソナー君！！-----------

			drawFillBox(-225, -500, 250, 200, Color(1, 1, 1));
			drawFillBox(-220, -495, 240, 192, Color(0, 0, 0));
			drawLine(-225, -390, 25, -390, 3, Color(1, 1, 1, 0.3));
			drawLine(-170, -500, -170, -300, 2, Color(1, 1, 1, 0.3));
			drawLine(-30, -500, -30, -300, 2, Color(1, 1, 1, 0.3));
			float player_x = (player.x * 2) / 11;

			drawFillBox(player_x - 87, -390, 26, 6, Color(1, 1, 1));

			for (int i = 0; i < ENEMY_MAX; i++) {
				if (enemy[i].alive == 0)continue;

				if (enemy[i].alive == 1) {
					float x, y;
					x = (enemy[i].x / 5) - 100;
					y = (enemy[i].y / 5) - 450;
					drawFillBox(x, y, 11, 6, Color(1, 1, 1));
				}

			}

			//---------スコア表示-------------
			char buffer[100];
			sprintf(buffer, "%06d", player.score);
			font.draw(buffer, Vec2f(320, 150), Color(1, 1, 1));
			font.draw("SCORE", Vec2f(290, 185), Color(1, 0, 0));
			env.end();

		}
		env.flushInput();


	gameover:
		env.flushInput();
		if (screen.gameover == true) {
			piano.play();
			//huwahuwa.stop();
			sonar.stop();
		}
		while (screen.gameover) {
			//ゲームオーバー
			env.begin();
			screen.hit = false;
			drawTextureBox(-1000, -500, 2000, 1000, 0, 0, 1024, 512, gameoverback);
			if (env.isKeyPushed(' ')) {
				system.play();
				screen.gameover = false;
				screen.title = true;
				goto title;
				break;
			}
			env.end();

		}

		env.flushInput();

		env.end();
	}
}