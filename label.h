#include "header.h"

using namespace std;

class Label {
	private:
		const char* text;
		float x;
		float y;
		Font font;
		int font_size;
		Color font_color;
		int spacing = 1;

	public:
		Label(const char* text, float x, float y, Font font, int font_size, Color font_color) {
			this->text = text;
			this->x = x;
			this->y = y;
			this->font = font;
			this->font_size = font_size;
			this->font_color = font_color;
		}
		
		int getLength() {
			return MeasureTextEx(font, text, font_size, spacing).x;
		}

		void setText(const char* text) {
			this->text = text;
		}

		void render() {
			DrawTextEx(font, text, {x, y}, font_size, spacing, font_color);
		}
};
