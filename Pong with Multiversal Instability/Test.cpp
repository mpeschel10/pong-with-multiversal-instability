// Generated with Python
struct Point reflectTests[] = {
#include "ReflectTestData.cpp"
};

struct IntersectionTest {
	LineSegment l1, l2;
	struct Point p;
};

IntersectionTest intersectionTests[] = {
	#include "IntersectionTestData.cpp"
};


void test() {
	for (int i = 0; i < sizeof(reflectTests) / sizeof(struct Point); i += 3) {
		struct Point incident = reflectTests[i];
		struct Point normal = reflectTests[i + 1];
		
		struct Point expected_reflection = reflectTests[i + 2];
		struct Point observed_reflection = reflect(incident, normal);
		if (!approximates(expected_reflection, observed_reflection, 0.001)) {
			std::cerr << "Error on reflection test " << i << ": reflect " << incident << " over " << normal
				<< ": Expected " << expected_reflection << " but got " << observed_reflection << "." << std::endl;
		}

		expected_reflection = reflectTests[i + 2] * 3.7;
		observed_reflection = reflect(incident * 3.7, normal);
		if (!approximates(expected_reflection, observed_reflection, 0.001)) {
			std::cerr << "Error on reflection test " << i << " * 3.7: reflect " << incident << " over " << normal
				<< ": Expected " << expected_reflection << " but got " << observed_reflection << "." << std::endl;
		}
	}

	for (int i = 0; i < sizeof(intersectionTests) / sizeof(struct IntersectionTest); i++) {
	LineSegment l1 = intersectionTests[i].l1;
	LineSegment l2 = intersectionTests[i].l2;
	struct Point expected_intersection = intersectionTests[i].p;
	struct Point observed_intersection = l1.wouldIntersect(l2);
		
	if (!approximates(expected_intersection, observed_intersection, 0.001)) {
	std::cerr << "Error on intersection test " << i << ": l1 " << l1 << " vs l2 " << l2
	<< ": Expected " << expected_intersection << " but got " << observed_intersection << "." << std::endl;
	}

	}
}