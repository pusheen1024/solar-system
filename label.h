#include "header.h"

using namespace std;

class Label {
	protected:
		char* text;
		float x;
		float y;
		Font font;
		int font_size;
		Color font_color;
		int spacing = 1;

	public:
		Label() {}

		Label(const char* info, float x, float y, Font font, int font_size, Color font_color) {
			this->setText(info);
			this->x = x;
			this->y = y;
			this->font = font;
			this->font_size = font_size;
			this->font_color = font_color;
		}
		
		int getLength() {
			return MeasureTextEx(font, text, font_size, spacing).x;
		}

		bool isInside(Vector2 coords) {
			return (x <= coords.x && coords.x <= x + getLength() && y <= coords.y && coords.y <= y + font_size);
		}

		void setText(const char* info) {
			text = new char[strlen(info) + 1];
            strcpy(this->text, info);
		}

		void render() {
			DrawTextEx(font, text, {x, y}, font_size, spacing, font_color);
		}
};

class LabelWithText: public Label {
	private:
		char* sub_text;

	public:
	LabelWithText() : Label() {}

	LabelWithText(const char* info, const char* sub_info, float x, float y, Font font, int font_size, Color font_color) :
	Label(info, x, y, font, font_size, font_color) {
		sub_text = new char[strlen(sub_info) + 1];
		strcpy(this->sub_text, sub_info);
	}

	int getSubTextLength() {
		return MeasureTextEx(font, sub_text, font_size, spacing).x;
	}

	void render_info() {
		auto [x, y] = GetMousePosition();
		x -= this->getSubTextLength() / 2;
		DrawRectangle(x, y, this->getSubTextLength(), font_size * 2, RAYWHITE);
		DrawRectangleLines(x, y, this->getSubTextLength(), font_size * 2, GRAY);
		DrawTextEx(font, sub_text, {x, y}, font_size, spacing, BLACK);
	}

	bool showText() {
		if (this->isInside(GetMousePosition())) {
			this->render_info();
			return 1;
		}
		return 0;
	}
};
