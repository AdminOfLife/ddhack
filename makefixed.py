import xml.dom.minidom
import re
from PIL import Image

print "#include <hash_map>"
print
print "static unsigned char *fixed_data[0x7f - 0x20];"
print "static unsigned int fixed_width[0x7f - 0x20];"
print "static unsigned int fixed_height[0x7f - 0x20];"
print "static unsigned int fixed_aw[0x7f - 0x20];"
print "static std::hash_map<short, int> fixed_kernings;"
print
print "#define X 1,"
print "#define _ 0,"
print

im = Image.open("test-0.png")
doc = xml.dom.minidom.parse("test.xml")
for i in doc.getElementsByTagName("glyph"):
	r = re.compile('[,x]')
	charcode = int(i.getAttribute("code"), 16)
	x, y = r.split(i.getAttribute("origin"))
	width, height = r.split(i.getAttribute("size"))
	x = int(x)
	y = int(y)
	width = int(width)
	height = int(height)
	aw = int(i.getAttribute("aw"))
	
	print "static unsigned char fixed_{:02x}_data[] = {{".format(charcode)
	for j in range(y, y + height):
		for k in range(x, x + width):
			pixel = im.getpixel((k, j))
			if pixel == (0, 0, 0, 0):
				pixel = "_"
			else:
				pixel = "X"
			print "{}".format(pixel),
		print
	print "};"
	print "static unsigned int fixed_{:02x}_width = {};".format(charcode, width)
	print "static unsigned int fixed_{:02x}_height = {};".format(charcode, height)
	print "static unsigned int fixed_{:02x}_aw = {};".format(charcode, aw)
	print

print "#undef X"
print "#undef _"
print
print "void fixed_kernings_setup()"
print "{"

for i in range(0x20, 0x7f):
	print "	fixed_data[{:#04x}] = fixed_{:02x}_data;".format(i - 0x20, i)
	print "	fixed_width[{:#04x}] = fixed_{:02x}_width;".format(i - 0x20, i)
	print "	fixed_height[{:#04x}] = fixed_{:02x}_height;".format(i - 0x20, i)
	print "	fixed_aw[{:#04x}] = fixed_{:02x}_aw;".format(i - 0x20, i)

print

for i in doc.getElementsByTagName("kernpair"):
	left = ord(i.getAttribute("left"))
	right = ord(i.getAttribute("right"))
	adjust = int(i.getAttribute("adjust"))
	kerncode = left | (right << 8)
	print "	fixed_kernings[{:#06x}] = {};".format(kerncode, adjust)

print "}"
