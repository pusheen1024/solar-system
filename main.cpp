#include "header.h"
#include "textbox.h"
#include "label.h"
#include "button.h"
#include "planets.h"

using namespace std;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const ptt CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

vector<const char*> paths = {"sun.png", "mercury.png", "venus.png", "earth.png", "mars.png", "jupiter.png", "saturn.png", "uranus.png", "neptune.png"};
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
		//render_image(SUN, CENTER);
		EndDrawing();
    }
    CloseWindow(); 
    return 0;
}
