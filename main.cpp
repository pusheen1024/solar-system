#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 400;
const int PAGINATION = 5;
const Vector2 CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

const float ZOOM = 0.025; // начальное приближение камеры
const float MIN_COEFF = 0.5;
float COEFF = 500; // скорость движения
float SCALE = 2.5; // масштаб для расстояний
const ld EPS = 1e-9;
const ld G = 6.67e-11; // гравитационная постоянная

mt19937 rnd(time(NULL));

const int SPACING = 1; // параметр для шрифта
Font font;

Color inc_color = Color({193, 142, 240, 255});
Color dec_color = Color({240, 142, 209, 255});
Color show_color = Color({177, 238, 134, 255});
Color hide_color = Color({238, 134, 139, 255});
Color btn_color = Color({181, 98, 228, 255});
Color font_color = Color({86, 19, 124, 255});
Color error_color = Color({158, 26, 47, 255});
Color textbox_color = Color({234, 216, 243, 255});
Color help_rect = Color({221, 213, 213, 255});

vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", 
							 "saturn.png", "uranus.png", "neptune.png", "moon.png", "phobos.png", "deimos.png",
							 "io.png", "europe.png", "ganymede.png", "callisto.png", "comet.png"};
vector<Image> images(paths.size());
vector<Texture2D> textures(paths.size());
vector<int> show_object(paths.size(), 1);

void load_images() {
	for (int i = 0; i < paths.size(); i++) {
		const char* path = TextFormat("assets/%s", paths[i]);
		Image image = LoadImage(path);
		images[i] = image;
		Texture2D texture = LoadTextureFromImage(image);
		textures[i] = texture;
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

ld kepler(ld M, ld e) {
	ld E = M;
	for (int i = 0; i < 100; i++) {
		ld f = E - e * sin(E) - M;
		ld f_deriv = 1 - e * cos(E);
		ld d = f / f_deriv;
		E -= d;
		if (abs(d) < EPS) break;
	}
	return E;
}

class CosmicObject {
	protected:
		ld mass;
		float diam;
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

		ld getMass() {return mass; }
		
		const char* getName() {return name; }
		virtual const char* getType() {return type; }

		const char* getInfo() {
			return TextFormat("%s - %s\nс массой %Le кг.", name, this->getType(), mass); 
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
			float scale = (picture_id ? 1e2 : 1e5);
			image_width = image_height = diam / scale;
			auto [image_x, image_y] = this->getCoords();	
			Texture2D texture = textures[picture_id];
			Rectangle src = {0, 0, (float)texture.width, (float)texture.height};
			Rectangle dest = {image_x, image_y, diam / scale, diam / scale};
			DrawTexturePro(texture, src, dest, {0, 0}, 0, WHITE);
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
		Color orbit_color;

	public:
		RotatingObject() : CosmicObject() {}

		virtual ld getA() {return a + image_width / 2; }
		virtual ld getB() {return getA() * sqrt(1 - e * e); }
		
		virtual float center_x() {return 0; }
		virtual float center_y() {return 0; }

		void updateCoords(ld t) {
			if (picture_id == 2) t *= (-1); // у Венеры ретроградная орбита
			ld M = 2 * PI * (t / (COEFF * T));
			while (M > 2 * PI) M -= 2 * PI;	
			ld E = kepler(M, e);
			x = center_x() + getA() * (cos(E) - e);
			y = center_y() + getB() * sin(E);
		}

		void drawOrbit() {
			if (! show_object[picture_id]) return;
			DrawEllipseLines(center_x(), center_y(), this->getA(), this->getB(), orbit_color);
		}
};

class Planet: public RotatingObject {
	public:
		Planet(): RotatingObject() {
			this->type = "планета";
			this->orbit_color = BLUE;
		}

		float center_x() {return CENTER.x; }
		float center_y() {return CENTER.y; }
};

class Mercury: public Planet {
	public:
		Mercury() : Planet() {
			this->mass = 3.285e23;
			this->picture_id = 1;
			this->a = 57.91 * SCALE;
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
			this->a = 108.2 * SCALE;
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
			this->a = 150 * SCALE;
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
			this->a = 228 * SCALE;
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
			this->a = 778 * SCALE;
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
			this->a = 1429 * SCALE;
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
			this->a = 2875 * SCALE;
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
			this->a = 4497 * SCALE;
			this->e = 0.0086;
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
			this->orbit_color = PURPLE;
		}
		
		ld getA() {
			return a + planet->image_width / 2 + image_width / 2;
		}

		ld getB() {
			return a * sqrt(1 - e * e) + planet->image_height / 2 + image_height / 2;
		}

		float center_x() {return planet->x; }
		float center_y() { return planet->y; }

		const char* getType() {
			return TextFormat("%s планеты %s", type, planet->getName());
		} 
};

class Moon: public Satellite {
	public:
		Moon(Planet* earth) : Satellite(earth) {
			this->mass = 7.36e22;
			this->picture_id = 9;
			this->a = 384748.0 / 1e7 * SCALE;
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
			this->a = 9377.0 / 1e7 * SCALE;
			this->e = 0.015;
			this->diam = 22.53 * 10;
			this->T = 7.65 / 24 / 365;
			this->name = "Фобос";
		}
};

class Deimos: public Satellite {
	public:
		Deimos(Planet* mars) : Satellite(mars) {
			this->mass = 1.48e15;
			this->picture_id = 11;
			this->a = 23458 / 1e7 * SCALE;
			this->e = 0.0002;
			this->diam = 12.4 * 10;
			this->T = 1.2624 / 365;
			this->name = "Деймос";
		}
};

class Io: public Satellite {
	public:
		Io(Planet* jupiter) : Satellite(jupiter) {
			this->mass = 8.9319e22;
			this->picture_id = 12;
			this->a = 421800 / 1e7 * SCALE;
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
			this->a = 671034 / 1e7 * SCALE;
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
			this->a = 1.07 * SCALE;
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
	private:
		Vector2 velocity;
		float density; // плотность
		float scale; // масштаб
	
	public:
		Comet() : CosmicObject() {
			this->picture_id = 16;
			this->name = "Комета";
			this->density = 200;
			this->scale = 1e2;
		}

		Comet(ld mass, float velocity) : Comet() {
			this->x = 0;
			this->y = 0;
			this->velocity = {velocity, 0};
			this->setMass(mass);
		}
		
		// координаты моделируем случайным образом
		void setCoords() {
			this->x = rnd() % (WIDTH - BAR);
			this->y = rnd() % (HEIGHT);
		}

		void setMass(ld mass) {
			this->mass = mass;
			// V = 4/3*pi*R^3 = m/p (p - плотность)
			// D = 2R = 2 * sqrt3(m/p/(4/3 pi)) = 2 * sqrt3(0.75m/pi/p)
			this->diam = 2 * pow(0.75 * mass / PI / density, 1.0 / 3) * this->scale;
		}

		void setVelocity(Vector2 velocity) {
			this->velocity = velocity;
		}
	
		Vector2 getA(Vector2 pos, Sun *sun, vector<Planet*> &planets) { // ускорение кометы (через закон всемирного тяготения)
			float x = pos.x;
			float y = pos.y;
			float dx = (x - sun->x) * 1e7;
			float dy = (y - sun->y) * 1e7;
			float r = sqrt(dx * dx + dy * dy) * (dx * dx + dy * dy);
			Vector2 a = {0, 0};
			a.x -= G * sun->getMass() * dx / r;
			a.y -= G * sun->getMass() * dy / r;
			for (int i = 0; i < planets.size(); i++) {
				float dx = (x - planets[i]->x) * 1e7;
				float dy = (y - planets[i]->y) * 1e7;
				float r = sqrt(dx * dx + dy * dy) * (dx * dx + dy * dy);
				a.x -= G * planets[i]->getMass() * dx / r;
				a.y -= G * planets[i]->getMass() * dy / r;
			}
			return a;
		}
	
		pair<Vector2, Vector2> deriv(Vector2 pos, Vector2 velocity, Sun *sun, vector<Planet*> &planets) {
			return {velocity, getA(pos, sun, planets)};
		}

		// нахождение новых координат методом Рунге-Кутта: y(n + 1) = y(n) + h / 6 * (k1 + 2 * k2 + 2 * k3 + k4)
		void updateCoords(Sun *sun, vector<Planet*> &planets) {
			Vector2 a = this->getA({x, y}, sun, planets);
			float h = 0.05;
			auto k1 = deriv({x, y}, velocity, sun, planets);
			auto k2 = deriv({x + k1.first.x * h / 2, y + k1.first.y * h / 2}, velocity + k1.second * h / 2, sun, planets);
			auto k3 = deriv({x + k2.first.x * h / 2, y + k2.first.y * h / 2}, velocity + k2.second * h / 2, sun, planets);
			auto k4 = deriv({x + k3.first.x * h, y + k3.first.y * h}, velocity + k3.second * h, sun, planets);
			float ox = x, oy = y;
			x += (h / 6) * (k1.first.x + k2.first.x * 2 + k3.first.x * 2 + k4.first.x);
			y += (h / 6) * (k1.first.y + k2.first.y * 2 + k3.first.y * 2 + k4.first.y);
			velocity += (k1.second + k2.second * 2 + k3.second * 2 + k4.second) * h / 6;
		}
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Компьютерная модель Солнечной системы");
    SetTargetFPS(60); 
	load_images();
	load_font();
	
	int x = WIDTH - BAR + PAGINATION;
	Label label_mass = Label("Масса кометы", x, 0, font, 30, font_color);
    TextBox input_mass = TextBox(x + 15 + label_mass.getLength(), 0, 30, font_color, textbox_color);
	Label label_velocity = Label("Скорость кометы", x, 40, font, 30, font_color);
	TextBox input_velocity = TextBox(x + 15 + label_velocity.getLength(), 40, 30, font_color, textbox_color);
	Button comet_button = Button("Смоделировать перелёт кометы", x, 80, font, 30, font_color, btn_color);
	Label label_error = Label("", x, 115, font, 30, error_color);
	Button inc_speed = Button("Ускорить движение", x, 160, font, 30, font_color, inc_color);
	Button dec_speed = Button("Замедлить движение", x + 195, 160, font, 30, font_color, dec_color);
	LabelWithBG label_info = LabelWithBG("Показать справку",
										 "Чтобы приблизить, используйте колёсико мыши,\n"
							 			 "Чтобы переместить, зажмите правую кнопку мыши.\n"
							 			 "Чтобы скрыть название планеты, нажмите на неё.\n"
			 				 			 "Чтобы вернуться к исходному\n"
							 			 "состоянию камеры, нажмите R.", 5, 
										 x + 55, 450, font, 50, 25, error_color, hide_color);


	Comet comet = Comet();
	bool show_comet = 0;
	auto model_comet = [&]() {
		ld mass = input_mass.getValue();
		float velocity = input_velocity.getValue();
		if (mass == 0 || velocity == 0) {
			label_error.setText("Ошибка: задано нулевое значение!");
			return;
		}
		label_error.setText("Моделирование выполнено.");
		comet.setCoords();
		comet.setMass(mass);
		comet.setVelocity({velocity, 0});
		show_comet = 1;
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

	// чекбоксы для отображения планет
	vector<LabelWithText> labels(n);
	vector<CheckBox> checkboxes(n);
	vector<const char*> texts = {"Скрыть", "Отобразить"};
	vector<Color> colors = {show_color, hide_color};
	float y = 200;
	for (int i = 0; i < n; i++) {
		auto info = objects[i]->getInfo();
		labels[i] = LabelWithText(objects[i]->getName(), objects[i]->getInfo(), 2, x, y, font, 25, 25, font_color);
		checkboxes[i] = CheckBox(texts, x + 80, y, font, 25, BLACK, colors);
		y += 30; // располагаем в 2 столбца
		if (y > 200 + 30 * 7) {
			x += 200;
			y = 200;
		}
	}
	ld t = 0;
	Camera2D camera = {0};
	auto restart_camera = [&]() { // перезапуск камеры
		camera.zoom = ZOOM;
		camera.offset = CENTER;
		camera.target = CENTER;
	};
	restart_camera();
    while (!WindowShouldClose()) {
		BeginDrawing();
        ClearBackground(BLACK);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			input_mass.setCursor();
			input_velocity.setCursor();
			if (comet_button.click()) model_comet();
			for (int i = 0; i < n; i++) {
				if (checkboxes[i].toggle()) show_object[i] ^= 1;
			}
			Vector2 real_pos = GetScreenToWorld2D(GetMousePosition(), camera);
			for (auto obj : objects) { 
				obj->showText(real_pos);
			}
			if (inc_speed.click() && COEFF >= MIN_COEFF * 2) COEFF /= 2;
			else if (dec_speed.click()) COEFF *= 2;
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta *= (-1 / camera.zoom);
			camera.target += delta;
		}
		float wheel = GetMouseWheelMove();
		if (wheel != 0) {
			auto pos = GetScreenToWorld2D(GetMousePosition(), camera);
			camera.offset = GetMousePosition();
			camera.target = pos;
			camera.zoom += (wheel * ZOOM);
			if (camera.zoom < ZOOM) camera.zoom = ZOOM;
		}
		if (input_mass.isActive()) {
			input_mass.handleKeyboard();
		}
		else if (input_velocity.isActive()) {
			input_velocity.handleKeyboard();
		}
		if (IsKeyPressed(KEY_R)) {
			restart_camera();
		}
		BeginMode2D(camera);
		for (auto planet : planets) {
			(*planet).updateCoords(t);	label_info.render();
			(*planet).drawOrbit();
		}
		for (auto satellite : satellites) {
			(*satellite).updateCoords(t);
			(*satellite).drawOrbit();
		}
		for (auto obj : objects) (*obj).render();
		if (show_comet) {
			comet.updateCoords(&sun, planets);
			comet.render();
		}
		EndMode2D();
		DrawRectangle(WIDTH - BAR, 0, BAR, HEIGHT, WHITE);
		label_mass.render();
		input_mass.render();
		comet_button.render();
		inc_speed.render();
		dec_speed.render();
		label_velocity.render();
		input_velocity.render();
		label_error.render();
		label_info.render();
		bool show_text = 0;
		for (int i = 0; i < n; i++) labels[i].render();
		for (auto chkbx : checkboxes) (&chkbx)->render();
		for (int i = 0; i < n; i++) {
			if (labels[i].showText()) break;
		}
		label_info.showText();
		EndDrawing();
		t += 0.05;
	}
    CloseWindow(); 
}
