import random
from math import pi, sin, cos

def angles():
	angle = 0
	while angle < 2 * pi:
		yield angle
		angle += 2 * pi / 10

class Point:
	def __init__(self, x, y):
		self.x = x; self.y = y
	def random(self):
		return Point(random.random() * self.x, random.random() * self.y)
	def __eq__(self, other):
		if other is None: return False
		if type(self) != type(other): return False
		return self.x == other.x and self.y == other.y
	def __str__(self):
		return f'Point {{{self.x}, {self.y}}}'
	def __rmul__(self, other):
		if type(other) == float:
			return Point(self.x * other, self.y * other)
		else:
			raise Exception()
	def __add__(self, other):
		if type(other) == Point:
			return Point(self.x + other.x, self.y + other.y)
		else:
			raise Exception()
	def __truediv__(self, other):
		if type(other) == float or type(other) == int:
			return Point(self.x / other, self.y / other)
		else:
			raise Exception()
	
	def length(self):
		return (self.x * self.x + self.y * self.y) ** 2
	def normalized(self):
		return self / self.length()
	
	def is_parallel(self, other):
		return self.x * other.y - self.y * other.x == 0

class LineSegment:
	def __init__(self, start, offset):
		self.start = start; self.offset = offset
	def __str__(self):
		direction = self.offset.normalized()
		length = self.offset.length()
		return f'LineSegment {{{self.start}, {direction}, {length}}}'


def angle_to_point(angle):
	return f'{{{round(cos(angle), 6)}, {round(sin(angle), 6)}}}'

def make_reflect_tests():
	p = angle_to_point
	for normal in angles():
		for incident in angles():
			offset_to_horizontal = -normal
			normalized_normal = normal + offset_to_horizontal
			normalized_incident = incident + offset_to_horizontal
			normalized_reflection = -normalized_incident
			reflection = normalized_reflection - offset_to_horizontal

			yield f'{p(incident)}, {p(normal)}, {p(reflection)},'

def make_intersect_tests():
	random.seed(531)
	ranges_of_interest = [
		Point(10, 10), Point(-10, 10), Point(10, -10), Point(-10, -10),
		Point(10, 0), Point(-10, 0), Point(0, -10), Point(0, 10)
	]
	
	points_of_interest = [
		(0, 0),
		(0, 1), (0, -1), (1, 0), (-1, 0),
		(1, 1), (-1, -1), (1, -1), (-1, 1)
	]
	points_of_interest = [Point(x, y) for x, y in points_of_interest]
	points_of_interest += [p.random() for p in ranges_of_interest for _ in range(3)]

	offsets_of_interest = [
		(0, 1), (0, -1), (1, 0), (-1, 0),
		(1, 1), (-1, -1), (1, -1), (-1, 1)
	]
	offsets_of_interest = [Point(x, y) for x, y in offsets_of_interest]
	offsets_of_interest += [p.random() for p in ranges_of_interest for _ in range(3)]

	parallel_misses = []
	parallel_hits = []
	for offset in offsets_of_interest:
		s1 = random.choice(points_of_interest)
		s2 = random.choice(points_of_interest)
		while s2 == s1:
			s2 = random.choice(points_of_interest)
		
		parallel_misses.append((LineSegment(s1, offset), LineSegment(s2, offset), None))
		parallel_hits.append((LineSegment(s1, offset), LineSegment(s1, offset), None))
	
	hits = []
	for p in points_of_interest:
		for o1 in offsets_of_interest:
			for o2 in offsets_of_interest:
				d1 = (random.random() - 0.5) * 2 * o1
				d2 = (random.random() - 0.5) * 2 * o2

				result = p if not o1.is_parallel(o2) else Point(-111, -111)
				hits.append((LineSegment(p + d1, o1), LineSegment(p + d2, o2), result))
	
	for case in parallel_hits + parallel_misses + hits:
		s1, s2, result = case
		no_hit = Point(-111, -111)
		if result == None:
			result = no_hit
		yield(f'IntersectionTest {{{s1}, {s2}, {result}}}')
	
	# Also add cases for offset == 0,0

def main():
	# list(make_intersect_tests())
	l = list(make_intersect_tests())
	random.shuffle(l)
	print(',\n'.join(l[:100]))

if __name__ == '__main__':
	main()
