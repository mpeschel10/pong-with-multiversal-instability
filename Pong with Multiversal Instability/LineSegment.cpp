#define NO_INTERSECTION Point {-111, -111}

class LineSegment {
	public:
		struct Point start;
		struct Point direction;
		float length = 1;

		struct Point point(float distance) { return start + direction * distance; }
		bool contains(float f) { return 0 <= f && f <= length; }

		// Derived from https://stackoverflow.com/a/565282/6286797
		float wouldIntersectFloat(const LineSegment& other) {
			struct Point start_difference = other.start - this->start;
			float toxoo = cross(this->direction, other.direction);

			if (toxoo == 0 || this->length == 0 || other.length == 0) {
				return -111; // Thorougly cursed
			}

			return cross(start_difference, other.direction) / toxoo;
		}
		
		struct Point wouldIntersect(const LineSegment& other) {
			float f = this->wouldIntersectFloat(other);
			if (f == -111) return NO_INTERSECTION;
			return this->point(f);
		}

		struct Point orthogonal() { return Point {-this->direction.y, this->direction.x}; }
		LineSegment orthogonal(struct Point base) { return LineSegment {base, this->orthogonal(), 1.0}; }

		float nearestFloat(struct Point base) { return wouldIntersectFloat(this->orthogonal(base)); }
		struct Point nearest(struct Point base) { return wouldIntersect(this->orthogonal(base)); }
		
		struct Point wouldIntersect(const Peg& p) {
			// To find the intersection point with circle p, draw a line orthogonal to us through the hub of p.
			LineSegment otherLineSegment = this->orthogonal(p.center);
			otherLineSegment.length = p.radius;
			// Find the intersection point of that line with us.
			float myDistance = this->wouldIntersectFloat(otherLineSegment);
			struct Point intersection = this->point(myDistance);
			
			float otherDistance = otherLineSegment.wouldIntersectFloat(*this);
			struct Point intersection2 = otherLineSegment.point(otherDistance);
			
			// Note that we have a right triangle now: the hub of the circle, the intersection with this line and the circle,
			//  and the intersection of this line and the line through the hub (which is at a right angle to this line).
			// We can find the intersections of this line and the circle by finding the linear distance along this line
			//  of the leg between the circle-line intersections and the line-line intersection.
			// The hypotenuse is a radius of the circle, and has that lengh.
			// The other leg is the distance from the hub to the line-line intersection.
			float gapSquared = p.radius * p.radius - otherDistance * otherDistance;
			if (gapSquared < 0) return NO_INTERSECTION;
			float gap = sqrt(gapSquared);

			// We need to choose which intersection. I assume length is always positive.
			return this->point(myDistance - gap);
		}

		void display(void) {
			struct Point end = start + direction * length;
			glColor3f(1,0,0);
			
			glBegin(GL_LINES);
			glVertex2f(start.x, start.y);
			glVertex2f(end.x, end.y);
			glEnd();
			
			glPointSize(12);
			pointDisplay(start);
			pointDisplay(end);
			
			glColor3f(1,1,1);
		}

};

std::ostream& operator<<(std::ostream& os, const LineSegment& l) {
	return os << "LineSegment {" << l.start << ", " << l.direction << "}";
}
