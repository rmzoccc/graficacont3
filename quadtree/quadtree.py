
from colormath.color_conversions import convert_color
from colormath.color_diff import delta_e_cie2000
from colormath.color_objects import LabColor, sRGBColor
from PIL import Image, ImageDraw


from math import floor, ceil
from enum import Enum



class Color(Enum):
	mix = 1
	no_determined = 2

class Rect:
	def __init__(self, x_i, y_i, x_f, y_f):
		self.x_i = x_i
		self.y_i = y_i
		self.x_f = x_f
		self.y_f = y_f
		self.color = Color.no_determined
		self.divisions = None

	def draw(self, image):
		if self.divisions is None:
			image = ImageDraw.Draw(image)
			image.rectangle([self.x_i, self.y_i, self.x_f, self.y_f],outline="red")
		else:
			for r in self.divisions:
				r.draw(image)


	def __str__(self):
		real_color = None
		if self.color is Color.mix 				: real_color = "MIX"
		elif self.color is Color.no_determined	: real_color = "No determined"
		else									: real_color = self.color
		return f"{self.x_i}, {self.y_i}, {self.x_f}, {self.y_f} \nColor: {real_color}"

def get_diff(first_color, second_color):
	color1 = sRGBColor(first_color[0], first_color[1], first_color[2])
	color2 = sRGBColor(second_color[0], second_color[1], second_color[2])

	color1 = convert_color(color1, LabColor);
	color2 = convert_color(color2, LabColor);

	delta_e = delta_e_cie2000(color1, color2)
	return delta_e

def has_color_diff(image, rect):
	pixel = image.getpixel((rect.x_i, rect.y_i))
	for i in range(rect.x_i, rect.x_f):
		for j in range(rect.y_i, rect.y_f):
			diff = get_diff(pixel, image.getpixel((i, j)))
			pixel = image.getpixel((i, j))
			if diff >= 6: return (True, Color.mix)
	return (False, pixel)

def get_division(image, rect):
	has_diff, color = has_color_diff(image, rect)
	if has_diff:
		rect.color = Color.mix

		x_i, y_i, x_f, y_f = (rect.x_i, rect.y_i, rect.x_f, rect.y_f)
		x_mid, y_mid = floor((x_f-x_i)/2.0), floor((y_f-y_i)/2.0)
		first_div = Rect(x_i, y_i, x_i+x_mid, y_i+y_mid)
		second_div = Rect(x_i+x_mid, y_i, x_f, y_i+y_mid)
		third_div = Rect(x_i, y_i+y_mid, x_i+x_mid, y_f)
		fourth_div = Rect(x_i+x_mid, y_i+y_mid, x_f, y_f)

		rect.divisions = [first_div, second_div, third_div, fourth_div]

		for r in rect.divisions:
			get_division(image, r)
	else:
		rect.color = color

image = Image.open("dbz.png")


rgb_image = image.convert('RGB')
width, height = rgb_image.size


quadtree = Rect(0, 0, width, height)
get_division(rgb_image, quadtree)

quadtree.draw(rgb_image)
rgb_image.show()