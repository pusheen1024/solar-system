#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"
#include <cstring> 

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const Vector2 CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

ld COEFF = 500; // скорость движения
const ld EPS = 1e-9;

const int SPACING = 1; // параметр для шрифта
Font font;

vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", 
							 "saturn.png", "uranus.png", "neptune.png", "moon.png", "phobos.png", "deimos.png",
							 "io.png", "europe.png", "ganymede.png", "callisto.png"};
vector<Image> images(paths.size());
vector<Texture2D> textures(paths.size());
vector<int> show_object(paths.size(), 1);

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
	public:
		ld mass;
		ld diam;
		int picture_id;
		bool is_resized = 0;
		bool show_text = 1;
		const char* name;
		const char* type;

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

		const char* getName() {
			return name;
		}

		virtual const char* getType() {
			return type;
		}

		const char* getInfo() {
			return TextFormat("%s - %s с массой %Le кг.", name, this->getType(), mass); 
		}

		Vector2 getCoords() {
			float image_x = x - image_width / 2;
			float image_y = y - image_height / 2;
			return {image_x, image_y};
		}

	    bool isInside(Vector2 coords) {
			auto [image_x, image_y] = this->getCoords();
			return (image_x <= coords.x && coords.x <= image_x + image_width &&
				   	image_y <= coords.y && coords.y <= image_y + image_height);
		}
	
		void showText(Vector2 mouse_pos) {
			if (this->isInside(mouse_pos))
				show_text ^= 1;
		}

		void render() {
			if (! show_object[picture_id]) return;
			Image image = images[picture_id];
			if (! is_resized) {
				int scale = 1e2;
				if (picture_id == 0) scale = 1e5;
				ImageResize(&image, diam / scale, diam / scale);
				Texture2D texture = LoadTextureFromImage(image);
				textures[picture_id] = texture;
				is_resized = 1;
			}
			Texture2D texture = textures[picture_id];
			image_width = texture.width;
			image_height = texture.height;
			auto [image_x, image_y] = this->getCoords();	
			DrawTexture(texture, image_x, image_y, WHITE);
			if (show_text)
				DrawTextEx(font, name, {image_x, image_y}, 20, SPACING, WHITE);
		}

		~CosmicObject() {}
};

class Sun: public CosmicObject {
	public:
		Sun() : CosmicObject() {
			this->mass = 1.9885e30;
			this->picture_id = 0;
			this->diam = 1392700;
			this->name = "Солнце";
			this->type = "звезда";
		}
};

class RotatingObject: public CosmicObject {
	protected:
		ld a; // большая полуось (в миллионах км)
		ld e; // эксцентриситет
		ld T; // период (в земных годах)

	public:
		RotatingObject() : CosmicObject() {}

		virtual ld getA() {return a + image_width / 2; }
		virtual ld getB() {return a * sqrt(1 - e * e) + image_height / 2; }

		virtual ld center_x() {return 0; }
		virtual ld center_y() {return 0; }

		void updateCoords(ld t) {
			ld M = 2 * PI * (t / (COEFF * T));
			while (M > 2 * PI) M -= 2 * PI;	
			auto E = kepler(M, e);
			x = center_x() + getA() * (cos(E) - e);
			y = center_y() + getB() * sin(E);
		}

		void drawOrbit() {
			if (! show_object[picture_id]) return;
			DrawEllipseLines(center_x(), center_y(), this->getA(), this->getB(), BLUE);
		}
};

class Planet: public RotatingObject {
	public:
		Planet(): RotatingObject() {
			this->type = "планета";
		}

		ld center_x() {
			return CENTER.x;
		}

		ld center_y() {
			return CENTER.y;
		}
};

class Mercury: public Planet {
	public:
		Mercury() : Planet() {
			this->mass = 3.285e23;
			this->picture_id = 1;
			this->a = 57.91;
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
			this->e = 0.2488;
			this->diam = 2376.6;
			this->T = 164.8;
			this->name = "Нептун";
		}
};

class Satellite: public RotatingObject {
	protected:
		Planet *planet; // планета, вокруг которой вращается
	
	public:
		Satellite() {}

		Satellite(Planet* planet) : RotatingObject() {
			this->planet = planet;
			this->type = "спутник";
		}
		
		ld getA() {
			return a + planet->image_width / 2 + image_width / 2;
		}

		ld getB() {
			return a * sqrt(1 - e * e) + planet->image_height / 2 + image_height / 2;
		}

		ld center_x() {
			return planet->x;
		}

		ld center_y() {
			return planet->y;
		}

		const char* getType() {
			return TextFormat("%s планеты %s", type, planet->getName());
		} 
};

class Moon: public Satellite {
	public:
		Moon(Planet* earth) : Satellite(earth) {
			this->mass = 7.36e22;
			this->picture_id = 9;
			this->a = 384748.0 / 1e7;
			this->e = 0.0549;
			this->diam = 3474.8;
			this->T = 27.32 / 365;
			this->name = "Луна";
		}
};

class Phobos: public Satellite {
	public:
		Phobos(Planet* mars) : Satellite(mars) {
			this->mass = 1.072e16;
			this->picture_id = 10;
			this->a = 9377.0 / 1e7;
			this->e = 0.015;
			this->diam = 22.53;
			this->T = 7.65 / 24 / 365;
			this->name = "Фобос";
		}
};

class Deimos: public Satellite {
	public:
		Deimos(Planet* mars) : Satellite(mars) {
			this->mass = 1.48e15;
			this->picture_id = 11;
			this->a = 23458 / 1e7;
			this->e = 0.0002;
			this->diam = 12.4;
			this->T = 1.2624 / 365;
			this->name = "Деймос";
		}
};

class Io: public Satellite {
	public:
		Io(Planet* jupiter) : Satellite(jupiter) {
			this->mass = 8.9319e22;
			this->picture_id = 12;
			this->a = 421800 / 1e7;
			this->e = 0.0041;
			this->diam = 3643.2;
			this->T = 1.769 / 365;
			this->name = "Ио";
		}
};

class Europe: public Satellite {
	public:
		Europe(Planet* jupiter) : Satellite(jupiter) {
			this->mass = 4.8017e22;
			this->picture_id = 13;
			this->a = 671034 / 1e7;
			this->e = 0.0094;
			this->diam = 3121.6;
			this->T = 3.55 / 365;
			this->name = "Европа";
		}
};

class Hanymede: public Satellite {
	public:
		Hanymede(Planet* jupiter) : Satellite(jupiter) {
			this->mass = 1.4819e23;
			this->picture_id = 14;
			this->a = 1.07;
			this->e = 0.0013;
			this->diam = 5262;
			this->T = 7.15 / 365;
			this->name = "Ганимед";
		}
};

class Callisto: public Satellite {
	public:
		Callisto(Planet* jupiter) : Satellite(jupiter) {
			this->mass = 1.075e23;
			this->picture_id = 15;
			this->a = 1.882;
			this->e = 0.0074;
			this->diam = 4820.6;
			this->T = 16.7 / 365;
			this->name = "Каллисто";
		}
};

class Comet: public CosmicObject {
	Comet(ld mass, ld velocity) : CosmicObject() {
		this->x = 0;
		this->y = 0;
		this->mass = mass;
		this->picture_id = 16;
		this->diam = this->mass / 100;
		this->name = "Комета";
	}

	void updateCoords() {
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
	Moon moon = Moon(&earth);
	Phobos phobos = Phobos(&mars);
	Deimos deimos = Deimos(&mars);
	Io io = Io(&jupiter);
	Europe europe = Europe(&jupiter);
	Hanymede hanymede = Hanymede(&jupiter);
	Callisto callisto = Callisto(&jupiter);
	vector<Planet*> planets = {&mercury, &venus, &earth, &mars, &jupiter, &saturn, &uranus, &neptune};
	vector<Satellite*> satellites = {&moon, &phobos, &deimos, &io, &europe, &hanymede, &callisto};
	vector<CosmicObject*> objects = {&sun};
	for (auto planet : planets) objects.push_back(planet);
	for (auto satellite : satellites) objects.push_back(satellite);
	int n = objects.size();
	vector<Label> labels(n);
	vector<CheckBox> checkboxes(n);
	vector<const char*> texts = {"Скрыть", "Отобразить"};
	vector<Color> colors = {RED, GREEN};
	int x = 180;
	for (int i = 0; i < n; i++) {
		auto info = objects[i]->getInfo();
		labels[i] = Label(info, WIDTH - BAR, x, font, 20, RED);
		checkboxes[i] = CheckBox(texts, WIDTH - BAR + 100, x, font, 20, BLACK, colors);
		x += 20;
	}
	ld t = 0;
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
		for (int i = 0; i < n; i++) labels[i].render();
		for (auto chkbx : checkboxes) 
			(&chkbx)->render();
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			input_mass.setCursor();
			input_velocity.setCursor();
			if (button.click()) model_comet();
			for (int i = 0; i < n; i++) {
				if (checkboxes[i].toggle()) show_object[i] ^= 1;
			}
			if (inc.click()) COEFF *= 2;
			else if (dec.click()) COEFF /= 2;
		}
		BeginMode2D(camera);
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
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {			
			Vector2 real_pos = GetScreenToWorld2D(GetMousePosition(), camera);
			sun.showText(real_pos);
			for (auto planet : planets) { 
				(*planet).showText(real_pos);
			}
		}
		sun.render();
		for (auto planet : planets) {
			(*planet).updateCoords(t);
			(*planet).drawOrbit();
		}
		for (auto satellite : satellites) {
			(*satellite).updateCoords(t);
			(*satellite).drawOrbit();
			(*satellite).render();
		}
		for (auto obj : objects) {
			(*obj).render();
		}
		EndMode2D();
		EndDrawing();
		t += 0.05;
	}
    CloseWindow(); 
    return 0;
}
