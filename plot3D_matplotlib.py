import pylab
import sys
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import proj3d

if len(sys.argv)!=2:
  print "Usage : ",sys.argv[0]," file3dpoints.txt"
  exit(-1)

fich = open(sys.argv[1])
lines = fich.readlines()
fich.close()

x=[]
y=[]
z=[]

for i in lines:
	tmp = i.split()
	x.append(float(tmp[0]))
	y.append(float(tmp[1]))
	z.append(float(tmp[2]))


fig = pylab.figure()
ax = fig.add_subplot(111, projection = '3d')

sc = ax.scatter(x,y,z)

ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")


x2s = range(len(x))
y2s = range(len(x))

for i in range(len(x)):
	x2s[i],y2s[i],_ = proj3d.proj_transform(x[i],y[i],z[i], ax.get_proj())

labels = range(len(x))

for i in range(len(x)):
	labels[i] = pylab.annotate( str(i+1), 
	xy = (x2s[i], y2s[i]), xytext = (-1, 1),
	textcoords = 'offset points', ha = 'right', va = 'bottom')

def update_position(e):
	for i in range(len(x)):
		x2s[i],y2s[i],_ = proj3d.proj_transform(x[i],y[i],z[i], ax.get_proj())
		labels[i].xy = x2s[i],y2s[i]
		labels[i].update_positions(fig.canvas.renderer)
	fig.canvas.draw()

fig.canvas.mpl_connect('button_release_event', update_position)
pylab.show()
