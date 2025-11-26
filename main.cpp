#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const Vector2 CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", "saturn.png", "uranus.png", "neptune.png"};
vector<Image> images(paths.size());
vector<Texture2D> textures(paths.size());

void load_images() {
	for (int i = 0; i < paths.size(); i++) {
		const char* path = TextFormat("assets/%s", paths[i]);
		Image image = LoadImage(path);
		images[i] = image;
	}
}

class CosmicObject {
	protected:
		ld mass;
		ld diam;
		int picture_id;
		bool is_resized = 0;

	public:
		int x;
		int y;

		CosmicObject() {
			this->x = CENTER.x;
			this->y = CENTER.y;
		}

		CosmicObject(Vector2 coords) {
			this->x = coords.x;
			this->y = coords.y;
		}

		ld getMass() {
			return mass;
		}

		void render() {
			Image image = images[picture_id];
			if (! is_resized) {
				int scale = 1e2;
				if (picture_id == 0) scale = 1e5;
				if (picture_id > 4) scale = 1e3;
				ImageResize(&image, diam / scale, diam / scale);
				Texture2D texture = LoadTextureFromImage(image);
				textures[picture_id] = texture;
				is_resized = 1;
			}
			Texture2D texture = textures[picture_id];
			int image_width = image.width;
			int image_height = image.height;
			DrawTexture(texture, x - image_width / 2, 
						y - image_height / 2, WHITE);
		}
};

class Sun: public CosmicObject {
	public:
		Sun() : CosmicObject() {
			this->mass = 1.9885e30;
			this->picture_id = 0;
			this->diam = 1392700;
		}
};

class Planet: public CosmicObject {
	protected:
		ld a; // большая полуось
		ld e; // эксцентриситет

	public:
		Planet(): CosmicObject() {}
		ld getB() {return a * sqrt(1 - e * e); }

		void updateCoords(ld t) {
			x = CENTER.x + a * 0.3 * cos(t);
			y = CENTER.y + getB() * 0.3 * sin(t);
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
		}
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Компьютерная модель Солнечной системы");
    SetTargetFPS(60); 
	load_images();
	Camera2D camera = {0};
	camera.zoom = 1;
	Label label_mass = Label("Mass", WIDTH - BAR, 0, 30, RED);
    TextBox input_mass = TextBox(WIDTH - BAR + 15 + label_mass.getLength(), 0, 30, BLACK, RED);
	Label label_velocity = Label("Velocity", WIDTH - BAR, 40, 30, RED);
	TextBox input_velocity = TextBox(WIDTH - BAR + 15 + label_velocity.getLength(), 40, 30, BLACK, RED);
	Button button = Button("Render", WIDTH - BAR + 120, 150, 30, BLACK, BLUE);
	Label label_error = Label("", WIDTH - BAR, 200, 30, RED);
	auto model_comet = [&]() {
		ld mass = input_mass.getValue();
		ld velocity = input_velocity.getValue();
		if (mass == 0 || velocity == 0) {
			label_error.setText("Error!");
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
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
		BeginMode2D(camera);
		DrawRectangle(WIDTH - BAR, 0, BAR, HEIGHT, WHITE);
		label_mass.render();
		input_mass.render();
		button.render();
		label_velocity.render();
		input_velocity.render();
		label_error.render();
		if (IsMouseButtonDown(0)) {
			input_mass.setCursor();
			input_velocity.setCursor();
			if (button.click()) model_comet();
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			Vector2 delta = GetMouseDelta();
			delta /= (-camera.zoom);
			camera.target += delta;
		}
		ld wheel = GetMouseWheelMove();
		if (wheel != 0) {
			auto pos = GetScreenToWorld2D(GetMousePosition(), camera);
			camera.offset = GetMousePosition();
			camera.target = pos;
			camera.zoom += (wheel * 0.2);
		}
		if (input_mass.isActive()) {
			input_mass.handleKeyboard();
		}
		else if (input_velocity.isActive()) {
			input_velocity.handleKeyboard();
		}
		sun.render();
		for (auto planet : planets) {
			(*planet).updateCoords(t);
		}
		for (auto planet : planets) {
			(*planet).render();
		}
		EndMode2D();
		EndDrawing();
		t += 0.05;
    }
    CloseWindow(); 
    return 0;
}
