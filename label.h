#include "header.h"
using namespace std;

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
			this->x = x;
			this->y = y;
			this->font_size = font_size;
			this->font_color = font_color;
		}
		
		int getLength() {
			return MeasureText(text, font_size);
		}

		void setText(const char* text) {
			this->text = text;
		}

		void render() {
			DrawText(text, x, y, font_size, font_color);
		}
};
