#define NO_INTERSECTION Point {-111, -111}

class LineSegment {
	public:
		struct Point start;
		struct Point offset;

		struct Point getStart() const { return start; }
		struct Point getOffset() const { return offset; }
		struct Point getEnd() const { return start + offset; }

		void setStart(const struct Point& p) { start = p; }
		void setOffset(const struct Point& p) { offset = p; }
		void setEnd(const struct Point& p) { offset = p - start; }

		struct Point offsetMultiple(float m) { return start + offset * m; }

		// Derived from https://stackoverflow.com/a/565282/6286797
		struct Point wouldIntersect(const LineSegment& other) {
			struct Point start_difference = other.start - this->start;
			float toxoo = cross(this->offset, other.offset);

			if (toxoo == 0) {
				return NO_INTERSECTION;
			}

			float c1 = cross(start_difference, other.offset) / toxoo;
			return this->offsetMultiple(c1);
		}
};

std::ostream& operator<<(std::ostream& os, const LineSegment& l) {
	return os << "LineSegment {" << l.start << ", " << l.offset << "}";
}
