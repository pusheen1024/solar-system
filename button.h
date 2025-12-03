#include "header.h"

class Button {
	public:
		const char* text;
		float x;
		float y; 
		Font font;
		int font_size;
		Color font_color;
		Color background_color;
		int spacing = 1;
		bool clicked = 0;
	
	public:
		Button() {}

		Button(const char* text, float x, float y, Font font, int font_size, Color font_color, Color background_color) {
			this->text = text;
			this->x = x;
			this->y = y;
			this->font = font;
			this->font_size = font_size;
			this->font_color = font_color;
			this->background_color = background_color;
		}
	
		int getLength() {
			return MeasureTextEx(font, text, font_size, spacing).x;
		}
		
	    bool isInside(Vector2 coords) {
			return (x <= coords.x && coords.x <= x + this->getLength() &&
				   	y <= coords.y && coords.y <= y + font_size);
		}

		bool click() {
			if (this->isInside(GetMousePosition())) {
				return clicked = 1;
			}
			return clicked = 0;
		}

		void render() {
			DrawRectangle(x, y, this->getLength(), font_size, background_color);
			DrawTextEx(font, text, {x, y}, font_size, spacing, font_color);
		}
};

class CheckBox {
	public:
		Button btn1;
		Button btn2;
		bool state = 0; 

	public:
		CheckBox() {
		}

		CheckBox(vector<const char*> texts, float x, float y, Font font, int font_size, Color font_color, vector<Color> bg_colors) {
			btn1 = Button(texts[0], x, y, font, font_size, font_color, bg_colors[0]);
			btn2 = Button(texts[1], x, y, font, font_size, font_color, bg_colors[1]);
		}

		void render() {
			if (state) btn2.render();
			else btn1.render();
		}

		bool toggle() {
			if (btn1.isInside(GetMousePosition())) {
				state ^= 1;
				return 1;
			}
			return 0;
		}
};
