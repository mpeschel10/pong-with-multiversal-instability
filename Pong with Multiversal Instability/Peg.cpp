#define CIRCLE_SEGMENT_COUNT 40
const float circleAngleLimit = 2 * 3.14159265358979323;
const float circleAngleOffset = circleAngleLimit / CIRCLE_SEGMENT_COUNT;

class LineSegment;

class Peg {
	public:
		struct Point center;
		float radius;

		struct Point angleToPoint(float angle) const {
			return center + pointAngleToPoint(angle) * radius;
		}
		
		void display(bool yellow = true) const {
			float angle = 0;
			if (yellow) glColor3f(1,1,0);
			glBegin(GL_POLYGON);
			for (float angle = 0; angle < circleAngleLimit; angle += circleAngleOffset) {
				struct Point p = this->angleToPoint(angle);
				glVertex2f(p.x, p.y);
			}
			glEnd();
			if (yellow) glColor3f(1,1,1);
		}

		LineSegment normal(const struct Point& p) const;
		// Implementation in LineSegment.cpp
};

std::ostream& operator<<(std::ostream& os, const Peg& peg) {
	return os << "Peg {" << peg.center << ", " << peg.radius << "}";
}


