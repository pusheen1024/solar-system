#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const ptt CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

//vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", "saturn.png", "uranus.png", "neptune.png"};
vector<const char*> paths = {"assets/sun.png"};
vector<Image> images(1);
vector<Texture2D> textures(1);

void load_images() {
	for (int i = 0; i < paths.size(); i++) {
		const char* path = paths[i];
		Image image = LoadImage(path);
		images[i] = image;
		Texture2D texture = LoadTextureFromImage(image);
		textures[i] = texture;
	}
}

class CosmicObject {
	protected:
		ld mass;
		int picture_id;

	public:
		CosmicObject() {}

		int x_coord;
		int y_coord;

		ld getMass() {
			return mass;
		}

		void render() {
			Image image = images[picture_id];
			Texture2D texture = textures[picture_id];
			int image_width = image.width;
			int image_height = image.height;
			DrawTexture(texture, x_coord - image_width / 2, 
						y_coord - image_height / 2, WHITE);
		}
};

class Sun: public CosmicObject {
	public:
		Sun() : CosmicObject() {
			this->mass = 1.9885e30;
			this->picture_id = 0;
		}
};

class Planet: public CosmicObject {
	public:
		Planet(): CosmicObject() {}
};

class Mercury: public Planet {
	public:
		Mercury() : Planet() {
			this->mass = 3.285e23;
			this->picture_id = 1;
		}
};

class Venus: public Planet {
	public:
		Venus() : Planet() {
			this->mass = 4.867e24;
			this->picture_id = 2;
		}
};

class Earth: public Planet {
	public:
		Earth() : Planet() {
			this->mass = 5.9742e24;
			this->picture_id = 3;
		}
};

class Mars: public Planet {
	public:
		Mars() : Planet() {
			this->mass = 6.39e23;
			this->picture_id = 4;
		}
};

class Jupiter: public Planet {
	public:
		Jupiter() : Planet() {
			this->mass = 1.8987e27;
			this->picture_id = 5;
		}
};

class Saturn: public Planet {
	public:
		Saturn() : Planet() {
			this->mass = 5.683e26;
			this->picture_id = 6;
		}
};

class Uranus: public Planet {
	public:
		Uranus() : Planet() {
			this->mass = 8.681e25;
			this->picture_id = 7;
		}
};

class Neptune: public Planet {
	public:
		Neptune() : Planet() {
			this->mass = 1.024e26;
			this->picture_id = 8;
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
	Sun sun = Sun();
	sun.x_coord = CENTER.x;
	sun.y_coord = CENTER.y;
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
		sun.x_coord = CENTER.x + 200 * cos(t);
		sun.y_coord = CENTER.y + 200 * sin(t);
		sun.render();
		EndDrawing();
		t += 0.05;
		if (t > 2 * PI) t -= 2 * PI;
    }
    CloseWindow(); 
    return 0;
}
