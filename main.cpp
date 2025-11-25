#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const ptt CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

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

		CosmicObject() {}

		CosmicObject(ptt coords) {
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
				if (picture_id == 0) scale = 1e4;
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
		Sun(ptt coords) : CosmicObject(coords) {
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
		Planet(ptt coords): CosmicObject(coords) {}
		ld getB() {return a * sqrt(1 - e * e); }

		void updateCoords(ld t) {
			x = CENTER.x + a * cos(t);
			y = CENTER.y + getB() * sin(t);
		}
};

class Mercury: public Planet {
	public:
		Mercury(ptt coords) : Planet(coords) {
			this->mass = 3.285e23;
			this->picture_id = 1;
			this->a = 57.91;
			this->e = 0.206;
			this->diam = 4879.4;
		}
};

class Venus: public Planet {
	public:
		Venus(ptt coords) : Planet(coords) {
			this->mass = 4.867e24;
			this->picture_id = 2;
			this->a = 108.2;
			this->e = 0.0068;
			this->diam = 12104;
		}
};

class Earth: public Planet {
	public:
		Earth(ptt coords) : Planet(coords) {
			this->mass = 5.9742e24;
			this->picture_id = 3;
			this->a = 150;
			this->e = 0.0167;
			this->diam = 12742;
		}
};

class Mars: public Planet {
	public:
		Mars(ptt coords) : Planet(coords) {
			this->mass = 6.39e23;
			this->picture_id = 4;
			this->a = 228;
			this->e = 0.00934;
			this->diam = 6779;
		}
};

class Jupiter: public Planet {
	public:
		Jupiter(ptt coords) : Planet(coords) {
			this->mass = 1.8987e27;
			this->picture_id = 5;
			this->a = 778;
			this->e = 0.049;
			this->diam = 139820;
		}
};

class Saturn: public Planet {
	public:
		Saturn(ptt coords) : Planet(coords) {
			this->mass = 5.683e26;
			this->picture_id = 6;
			this->a = 1429;
			this->e = 0.0557;
			this->diam = 116460;
		}
};

class Uranus: public Planet {
	public:
		Uranus(ptt coords) : Planet(coords) {
			this->mass = 8.681e25;
			this->picture_id = 7;
			this->a = 2875;
			this->e = 0.047;
			this->diam = 50724;
		}
};

class Neptune: public Planet {
	public:
		Neptune(ptt coords) : Planet(coords) {
			this->mass = 1.024e26;
			this->picture_id = 8;
			this->a = 4497;
			this->e = 0.2488;
			this->diam = 2376.6;
		}
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Компьютерная модель Солнечной системы");
    SetTargetFPS(60); 
	load_images();
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
	Sun sun = Sun(CENTER);
	Mercury mercury = Mercury({CENTER.x - 100, CENTER.y});
	Venus venus = Venus({CENTER.x - 200, CENTER.y});
	Earth earth = Earth({CENTER.x - 300, CENTER.y});
	Mars mars = Mars({CENTER.x - 400, CENTER.y});
	Jupiter jupiter = Jupiter({CENTER.x - 500, CENTER.y});
	Saturn saturn = Saturn({CENTER.x - 600, CENTER.y});
	Uranus uranus = Uranus({CENTER.x - 700, CENTER.y});
	Neptune neptune = Neptune({CENTER.x - 800, CENTER.y});
	vector<Planet*> planets = {&mercury, &venus, &earth, &mars, &jupiter, &saturn, &uranus, &neptune};
	ld t = 0;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
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
		EndDrawing();
		t += 0.05;
    }
    CloseWindow(); 
    return 0;
}
