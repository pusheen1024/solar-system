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
	
		int getLength() {
			return MeasureText(text, font_size);
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
			DrawText(text, x, y, font_size, font_color);
		}
};
