class Slider {
public:
	struct Point start;
	struct Point end;
	float handle;

	Slider() {
		start.x = 0.0f;
		start.y = 0.0f;
		end.x = 0.0f;
		end.y = 0.0f;
		handle = 1.0f;
	}

	Slider(float sX, float sY, float eX, float eY, float initHandle = 1.0f) {
		start.x = sX;
		start.y = sY;
		end.x = eX;
		end.y = eY;
		handle = initHandle;
	}

	void display() {

		glColor3f(0.75f, 0.75f, 0.75f);
		glRectf(start.x, start.y - 1, end.x, end.y + 1);
		glRectf(start.x - 2, start.y - 10, start.x, start.y + 10);
		glRectf(end.x, end.y - 10, end.x + 2, end.y + 10);
		glColor3f(1.0f, 1.0f, 1.0f);

		float handleX = (1 - handle) * start.x + handle * end.x;
		float handleY = (1 - handle) * start.y + handle * end.y;
		glRectf(handleX - 2.0f, handleY - 8.0f, handleX + 2.0f, handleY + 8.0f);
	}

	float setHandle(float x) {
		// Set the value of the handle based on the mouse (x) position 
		if (x < start.x) { x = start.x; }
		else if (x > end.x) { x = end.x; }
		handle = ((x - start.x) / (end.x - start.x));
		return handle;
	}

	Point getHandlePoint() {
		struct Point Handle;
		Handle.x = (1 - handle) * start.x + handle * end.x;
		Handle.y = (1 - handle) * start.y + handle * end.y;
		return Handle;
	}

	bool isWithinBounds(float x, float y) {
		return ((x >= (start.x - 5.0f)) && (x <= (end.x + 5.0f)) && (y >= (start.y - 10.0f)) && (y <= (end.y + 10.0f)));
	}
};