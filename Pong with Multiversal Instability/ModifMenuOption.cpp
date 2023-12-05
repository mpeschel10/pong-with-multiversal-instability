class modifMenuOption {
public:
	int modifier;
	Slider modifSlider;
	float x, y;

	modifMenuOption(int modif, float x, float y) {
		modifier = modif;
		this->x = x;
		this->y = y;
		modifSlider.start.x = x + 150.0f;
		modifSlider.start.y = y + 15.0f;
		modifSlider.end.x = x + 350.0f;
		modifSlider.end.y = y + 15.0f;
		modifSlider.handle = modifProbs[modifier];
	}

	void display() {
		renderText(descriptions[modifier], x, y);
		modifSlider.display();
	}

	bool isWithinBounds(float x, float y) {
		return modifSlider.isWithinBounds(x, y);
	}
};