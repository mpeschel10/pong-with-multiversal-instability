from math import pi, sin, cos

def angles():
	angle = 0
	while angle < 2 * pi:
		yield angle
		angle += 2 * pi / 10

def point(angle):
	return f'{{{round(cos(angle), 6)}, {round(sin(angle), 6)}}}'

def make_reflect_tests():
	for normal in angles():
		for incident in angles():
			offset_to_horizontal = -normal
			normalized_normal = normal + offset_to_horizontal
			normalized_incident = incident + offset_to_horizontal
			normalized_reflection = -normalized_incident
			reflection = normalized_reflection - offset_to_horizontal

			yield f'{point(incident)}, {point(normal)}, {point(reflection)},'

def main():
	print('\n'.join(make_reflect_tests()))

if __name__ == '__main__':
	main()
