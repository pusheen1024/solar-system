#include "header.h"
using namespace std;

class CosmicObject {
	private:
		ld mass;
		int picture_id;

	public:
		ld getMass() {
			return mass;
		}

		void render() {
			Image image = images[picture_id];
			Texture2D texture = textures[picture_id];
			int image_width = image.width;
			int image_height = image.height;
			DrawTexture(texture, coords.x - image_width / 2, coords.y - image_height / 2, WHITE);
		}
};

class Sun: CosmicObject {
	Sun() {
		this->mass = 1.9885e30;
		this->picture_id = 0;
	}
};

class Planet: CosmicObject {
};

class Mercury: Planet {
	Mercury() {
		this->mass = 3.285e23;
		this->picture_id = 1;
	}
};

class Venus: Planet {
	Venus() {
		this->mass = 4.867e24;
		this->picture_id = 2;
	}
};

class Earth: Planet {
	Earth() {
		this->mass = 5.9742e24;
		this->picture_id = 3;
	}
};

class Mars: Planet {
	Mars() {
		this->mass = 6.39e23;
		this->picture_id = 4;
	}
};

class Jupiter: Planet {
	Jupiter() {
		this->mass = 1.8987e27;
		this->picture_id = 5;
	}
};

class Saturn: Planet {
	Saturn() {
		this->mass = 5.683e26;
		this=>picture_id = 6;
	}
};

class Uranus: Planet {
	Uranus() {
		this->mass = 8.681e25;
		this=>picture_id = 7;
	}
};

class Neptune: Planet {
	Neptune() {
		this->mass = 1.024e26;
		this->picture_id = 8;
	}
};
