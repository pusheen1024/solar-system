#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <ctime>
#include "raylib.h"

#define x first
#define y second

using namespace std;

typedef pair<int, int> ptt;
typedef long double ld;

const int WIDTH = 1300;
const int HEIGHT = 600;
const int BAR = 350;
const ptt CENTER = {(WIDTH - BAR) / 2, HEIGHT / 2};

vector<const char*> paths = {"sun.png"};
vector<Image> images(1);
vector<Texture2D> textures(1);

const int SUN = 0;

void load_images() {
	for (int i = 0; i < paths.size(); i++) {
		const char* path = paths[i];
		Image image = LoadImage(path);
		images[i] = image;
		Texture2D texture = LoadTextureFromImage(image);
		textures[i] = texture;
	}
}

void render_image(int id, ptt coords) {
	Image image = images[id];
	Texture2D texture = textures[id];
	int image_width = image.width;
	int image_height = image.height;
	DrawTexture(texture, coords.x - image_width / 2, coords.y - image_height / 2, WHITE);
}

class Label {
	private:
		const char* text;
		int length;
		int font_size;
		int x;
		int y;
		Color font_color; 

	public:
		Label(const char* text, int x, int y, int font_size, Color font_color) {
			this->text = text;
			this->length = MeasureText(text, font_size);
			this->x = x;
			this->y = y;
			this->font_size = font_size;
			this->font_color = font_color;
		}
		
		int getLength() {
			return length;
		}

		void setText(const char* text) {
			this->text = text;
		}

		void render() {
			DrawText(text, x, y, font_size, font_color);
		}
};

class TextBox {
	private:
		char buffer[10] = {0};
		int font_size;
		int x;
   		int y;
		Color font_color;
		Color background_color;
		bool cursor = 0;
		int count = 0;
	
	public:
		TextBox(int x, int y, int font_size, Color font_color, Color background_color) {
			this->font_size = font_size;
			this->x = x;
			this->y = y;
			this->font_color = font_color;
			this->background_color = background_color;
		}
		
		void addDigit(int key) {
			if (key < int('0') || key > int('9')) return;
			addSymbol(key);
		}

		void addSymbol(int key) {
			if (count >= 10) return;
			if (cursor) count--;
			buffer[count] = char(key);
			count++;
			if (cursor) {
				buffer[count] = '|';
				count++;
			}
			buffer[count] = '\0';
		}

		void removeSymbol() {
			if (! count) return;
			if (! cursor) count--;
			else if (count > 1) {
				count--;
				buffer[count - 1] = '|';
			}
			buffer[count] = '\0';
		}

		void render() {
			DrawRectangle(x, y, 6 * font_size, font_size, background_color);
			DrawText(buffer, x, y, font_size, font_color);
		}

	    bool isInside(ptt coords) {
			return (x <= coords.x && coords.x <= x + 6 * font_size &&
				   	y <= coords.y && coords.y <= y + font_size);
		}

		bool isActive() {
			return cursor;
		}
		
		int getLength() {
			return MeasureText(buffer, font_size);
		}

		int getValue() {
			if (! this->buffer) return 0;
			return TextToInteger(this->buffer);
		}

		void setCursor() {
			if (this->isInside({GetMouseX(), GetMouseY()})) {
				if (! cursor) {
					this->addSymbol(int('|'));
					cursor = 1;
				}
			}
			else {
				if (cursor) {
					cursor = 0;
					this->removeSymbol();
				}
			}
		}

		void handleKeyboard() {
			int key = GetCharPressed();
			this->addDigit(key);
			if (IsKeyPressed(KEY_BACKSPACE)) {
				this->removeSymbol();
			}
		}
};

class Button {
	private:
		const char* text;
		int font_size;
		int x;
   		int y;
		Color font_color;
		Color background_color;
		bool clicked = 0;
	
	public:
		Button(const char* text, int x, int y, int font_size, Color font_color, Color background_color) {
			this->text = text;
			this->font_size = font_size;
			this->x = x;
			this->y = y;
			this->font_color = font_color;
			this->background_color = background_color;
		}
		
		void render() {
			DrawRectangle(x, y, this->getLength(), font_size, background_color);
			DrawText(text, x, y, font_size, font_color);
		}

	    bool isInside(ptt coords) {
			return (x <= coords.x && coords.x <= x + this->getLength() &&
				   	y <= coords.y && coords.y <= y + font_size);
		}
	
		int getLength() {
			return MeasureText(text, font_size);
		}

		bool isClicked() {
			return clicked;
		}

		void click() {
			if (this->isInside({GetMouseX(), GetMouseY()})) {
				clicked = 1;
			}
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
	Button button = Button("Render", WIDTH - BAR, 150, 30, BLACK, BLUE);
	Label label_error = Label("", WIDTH - BAR, 200, 30, RED);
	auto model_comet = [&]() {
		ld mass = input_mass.getValue();
		ld velocity = input_velocity.getValue();
		if (mass == 0 || velocity == 0) {
			label_error.setText("Error!");
		}
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
			button.click();
			if (button.isClicked()) model_comet();
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
