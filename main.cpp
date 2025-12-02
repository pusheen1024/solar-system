#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const int SPACING = 1;
int COEFF = 100;
const ld EPS = 1e-9;
const Vector2 CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", "saturn.png", "uranus.png", "neptune.png"};
vector<Image> images(paths.size());
vector<Texture2D> textures(paths.size());

Font font;

void load_images() {
	for (int i = 0; i < paths.size(); i++) {
		const char* path = TextFormat("assets/%s", paths[i]);
		Image image = LoadImage(path);
		images[i] = image;
	}
}

void load_font() {
	int codepoints[512] = { 0 };
	for (int i = 0; i < 95; i++) codepoints[i] = 32 + i;   // символы ASCII
	for (int i = 0; i < 255; i++) codepoints[96 + i] = 0x400 + i;   // кириллица
	font = LoadFontEx("segoeprint_bold.ttf", 32, codepoints, 512);
}

// E - e * sin(E) = M; ищем корни трансцендентного уравнения Кеплера методом Ньютона
// f(E) = E - e * sin(E) - M, f'(E) = 1 - e * cos(E)

float kepler(float M, float e) {
	float E = M;
	for (int i = 0; i < 100; i++) {
		float f = E - e * sin(E) - M;
		float f_deriv = 1 - e * cos(E);
		float d = f / f_deriv;
		E -= d;
		if (abs(d) < EPS) break;
	}
	return E;
}

class CosmicObject {
	protected:
		ld mass;
		ld diam;
		int picture_id;
		bool is_resized = 0;
		bool show_text = 1;
		const char* name;
		int font_size;

	public:
		float x;
		float y;
		float image_width;
		float image_height;

		CosmicObject() {
			this->x = CENTER.x;
			this->y = CENTER.y;
		}

		CosmicObject(Vector2 coords) {
			this->x = coords.x;
			this->y = coords.y;
		}

		int getLength() {
			return MeasureTextEx(font, name, font_size, SPACING).x;
		}

		Vector2 getCoords() {
			float image_x = x - image_width / 2;
			float image_y = y - image_height / 2;
			return {image_x, image_y};
		}

	    bool isInside(Vector2 coords) {
			auto [image_x, image_y] = this->getCoords();
			return (image_x <= coords.x && coords.x <= x + this->getLength() &&
				   	image_y <= coords.y && coords.y <= y + font_size);
		}
	
		void showText() {
			if (this->isInside(GetMousePosition()))
				show_text ^= 1;
		}

		void render() {
			Image image = images[picture_id];
			if (! is_resized) {
				int scale = 1e2;
				if (picture_id == 0) scale = 1e5;
				if (picture_id > 4) scale = 1e2;
				ImageResize(&image, diam / scale, diam / scale);
				Texture2D texture = LoadTextureFromImage(image);
				textures[picture_id] = texture;
				is_resized = 1;
			}
			Texture2D texture = textures[picture_id];
			image_width = image.width;
			image_height = image.height;
			auto [image_x, image_y] = this->getCoords();	
			DrawTexture(texture, x - image_width / 2, 
						y - image_height / 2, WHITE);
			if (show_text)
				DrawTextEx(font, name, {image_x, image_y}, 20, SPACING, WHITE);
		}
};

class Sun: public CosmicObject {
	public:
		Sun() : CosmicObject() {
			this->mass = 1.9885e30;
			this->picture_id = 0;
			this->diam = 1392700;
			this->name = "Солнце";
		}
};

class Planet: public CosmicObject {
	protected:
		ld a; // большая полуось
		ld e; // эксцентриситет
		ld T; // период

	public:
		Planet(): CosmicObject() {}
		ld getA() {return a; }
		ld getB() {return a * sqrt(1 - e * e); }

		void updateCoords(ld t) {
			ld M = 2 * PI * (t / (COEFF * T));
			while (M > 2 * PI) M -= 2 * PI;	
			auto E = kepler(M, e);
			x = CENTER.x + a * (cos(E) - e);
			y = CENTER.y + getB() * sin(E);
		}
};

class Mercury: public Planet {
	public:
		Mercury() : Planet() {
			this->mass = 3.285e23;
			this->picture_id = 1;
			this->a = 57.91;
			this->x -= this->a * 0.3;
			this->e = 0.206;
			this->diam = 4879.4;
			this->T = 0.241;
			this->name = "Меркурий";
		}
};

class Venus: public Planet {
	public:
		Venus() : Planet() {
			this->mass = 4.867e24;
			this->picture_id = 2;
			this->a = 108.2;
			this->x -= this->a * 0.3;
			this->e = 0.0068;
			this->diam = 12104;
			this->T = 0.615;
			this->name = "Венера";
		}
};

class Earth: public Planet {
	public:
		Earth() : Planet() {
			this->mass = 5.9742e24;
			this->picture_id = 3;
			this->a = 150;
			this->x -= this->a * 0.3;
			this->e = 0.0167;
			this->diam = 12742;
			this->T = 1;
			this->name = "Земля";
		}
};

class Mars: public Planet {
	public:
		Mars() : Planet() {
			this->mass = 6.39e23;
			this->picture_id = 4;
			this->a = 228;
			this->x -= this->a * 0.3;
			this->e = 0.00934;
			this->diam = 6779;
			this->T = 1.88;
			this->name = "Марс";
		}
};

class Jupiter: public Planet {
	public:
		Jupiter() : Planet() {
			this->mass = 1.8987e27;
			this->picture_id = 5;
			this->a = 778;
			this->x -= this->a * 0.3;
			this->e = 0.049;
			this->diam = 139820;
			this->T = 11.86;
			this->name = "Юпитер";
		}
};

class Saturn: public Planet {
	public:
		Saturn() : Planet() {
			this->mass = 5.683e26;
			this->picture_id = 6;
			this->a = 1429;
			this->x -= this->a * 0.3;
			this->e = 0.0557;
			this->diam = 116460;
			this->T = 29.46;
			this->name = "Сатурн";
		}
};

class Uranus: public Planet {
	public:
		Uranus() : Planet() {
			this->mass = 8.681e25;
			this->picture_id = 7;
			this->a = 2875;
			this->x -= this->a * 0.3;
			this->e = 0.047;
			this->diam = 50724;
			this->T = 84.02;
			this->name = "Уран";
		}
};

class Neptune: public Planet {
	public:
		Neptune() : Planet() {
			this->mass = 1.024e26;
			this->picture_id = 8;
			this->a = 4497;
			this->x -= this->a * 0.3;
			this->e = 0.2488;
			this->diam = 2376.6;
			this->T = 164.8;
			this->name = "Нептун";
		}
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Компьютерная модель Солнечной системы");
    SetTargetFPS(60); 
	load_images();
	load_font();
	Camera2D camera = {0};
	camera.zoom = 1;
	Label label_mass = Label("Масса кометы", WIDTH - BAR, 0, font, 30, RED);
    TextBox input_mass = TextBox(WIDTH - BAR + 15 + label_mass.getLength(), 0, 30, BLACK, RED);
	Label label_velocity = Label("Скорость кометы", WIDTH - BAR, 40, font, 30, RED);
	TextBox input_velocity = TextBox(WIDTH - BAR + 15 + label_velocity.getLength(), 40, 30, BLACK, RED);
	Button inc = Button("+", WIDTH - BAR, 80, font, 50, BLACK, BLUE);
	Button dec = Button("-", WIDTH - BAR + 100, 80, font, 50, BLACK, RED);
	Button button = Button("Смоделировать перелёт кометы", WIDTH - BAR, 150, font, 30, BLACK, BLUE);
	Label label_info = Label("Для приближения используйте колёсико мыши,\n для перемещения - правую кнопку мыши.", WIDTH - BAR, 400, font, 20, RED);
	Label label_error = Label("", WIDTH - BAR, 250, font, 30, RED);
	auto model_comet = [&]() {
		ld mass = input_mass.getValue();
		ld velocity = input_velocity.getValue();
		if (mass == 0 || velocity == 0) {
			label_error.setText("Ошибка!");
		}
		else label_error.setText("Ok");
	};
	Sun sun = Sun();
	Mercury mercury = Mercury();
	Venus venus = Venus();
	Earth earth = Earth();
	Mars mars = Mars();
	Jupiter jupiter = Jupiter();
	Saturn saturn = Saturn();
	Uranus uranus = Uranus();
	Neptune neptune = Neptune();
	vector<Planet*> planets = {&mercury, &venus, &earth, &mars, &jupiter, &saturn, &uranus, &neptune};
	ld t = 0;
	bool coeff_changed = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
		DrawRectangle(WIDTH - BAR, 0, BAR, HEIGHT, WHITE);
		label_mass.render();
		input_mass.render();
		button.render();
		inc.render();
		dec.render();
		label_velocity.render();
		input_velocity.render();
		label_error.render();
		label_info.render();
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			input_mass.setCursor();
			input_velocity.setCursor();
			if (button.click()) model_comet();
			if (inc.click() && COEFF < 300) {
				COEFF++;
				coeff_changed = 1;
			}
			else if (dec.click() && COEFF > 10) {
				COEFF--;
				coeff_changed = 1;
			}
			else coeff_changed = 0;
			for (auto planet : planets) 
				(*planet).showText();
		}
		else coeff_changed = 0;
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta *= (-1 / camera.zoom);
			camera.target += delta;
		}
		ld wheel = GetMouseWheelMove();
		if (wheel != 0) {
			auto pos = GetScreenToWorld2D(GetMousePosition(), camera);
			camera.offset = GetMousePosition();
			camera.target = pos;
			camera.zoom += (wheel * 0.2);
			if (camera.zoom < 0.2) camera.zoom = 0.2;
		}
		if (input_mass.isActive()) {
			input_mass.handleKeyboard();
		}
		else if (input_velocity.isActive()) {
			input_velocity.handleKeyboard();
		}
		BeginMode2D(camera);
		sun.render();
		for (auto planet : planets) {
			if (! coeff_changed) (*planet).updateCoords(t);
		}
		for (auto planet : planets) {
			(*planet).render();
		}
		EndMode2D();
		EndDrawing();
		if (! coeff_changed) t += 0.05;
	}
    CloseWindow(); 
    return 0;
}
