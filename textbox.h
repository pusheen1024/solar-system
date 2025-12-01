#include "header.h"    

class TextBox {
	private:
		char buffer[10] = {0};
		int count = 0;
		int font_size;
		int x;
   		int y;
		Color font_color;
		Color background_color;
		bool cursor = 0;
		bool dot = 0;
		
	public:
		TextBox(int x, int y, int font_size, Color font_color, Color background_color) {
			this->font_size = font_size;
			this->x = x;
			this->y = y;
			this->font_color = font_color;
			this->background_color = background_color;
		}
		
		void addDigit(int key) {
			if ((key < int('0') || key > int('9')) && (char)key != '.') return;
			addSymbol(key);
		}

		void addSymbol(int key) {
			if (count >= 10 || ((char)key == '.' && dot)) return;
			if (cursor) count--;
			buffer[count] = char(key);
			if (buffer[count] == '.') dot ^= 1;
			count++;
			if (cursor) {
				buffer[count] = '|';
				count++;
			}
			buffer[count] = '\0';
		}

		void removeSymbol() {
			if (! count) return;
			if (! cursor) {
				if (buffer[count] == '.') dot ^= 1;
				count--;
			}
			else if (count > 1) {
				count--;
				if (buffer[count - 1] == '.') dot ^= 1;
				buffer[count - 1] = '|';
			}
			buffer[count] = '\0';
		}
			
		int getLength() {
			return MeasureText(buffer, font_size);
		}

	    bool isInside(Vector2 coords) {
			return (x <= coords.x && coords.x <= x + 6 * font_size &&
				   	y <= coords.y && coords.y <= y + font_size);
		}	
		
		bool isActive() {	
			return cursor;
		}

		int getValue() {
			if (! this->buffer) return 0;
			return TextToFloat(this->buffer);
		}

		void setCursor() {
			if (this->isInside(GetMousePosition())) {
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

		void render() {
			DrawRectangle(x, y, 6 * font_size, font_size, background_color);
			DrawText(buffer, x, y, font_size, font_color);
		}
};

