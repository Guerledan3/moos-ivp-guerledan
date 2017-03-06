import numpy as np
import matplotlib.pyplot as plt
import random

theta = -6*np.pi/180.0
R = np.array([[np.cos(theta), np.sin(theta)],[-np.sin(theta), np.cos(theta)]])

corner1 = np.array([0, -100])
corner2 = np.array([201, 100])

L = 40
U = 20

w = corner2[0]-corner1[0]
h = corner1[1]-corner2[1]

n = int(np.ceil(abs(w/L)))
m = int(np.ceil(abs(h/L)))

if(n%2 != 0):
    n += 1
if(m%2 == 0):
    m += 1

print(n,m)

def get_top_lef(i, j):
    return np.array([corner1[0]+i*L+(L-U)/2, corner1[1]+(j+1)*L]).dot(R)

def get_top_rig(i, j):
    return np.array([corner1[0]+(i+1)*L-(L-U)/2, corner1[1]+(j+1)*L]).dot(R)

def get_bot_lef(i, j):
    return np.array([corner1[0]+i*L+(L-U)/2, corner1[1]+j*L]).dot(R)

def get_bot_rig(i, j):
    return np.array([corner1[0]+(i+1)*L-(L-U)/2, corner1[1]+j*L]).dot(R)

def get_lef_top(i, j):
    return np.array([corner1[0]+i*L, corner1[1]+(j+1)*L-(L-U)/2]).dot(R)

def get_lef_bot(i, j):
    return np.array([corner1[0]+i*L, corner1[1]+j*L+(L-U)/2]).dot(R)

def get_rig_top(i, j):
    return np.array([corner1[0]+(i+1)*L, corner1[1]+(j+1)*L-(L-U)/2]).dot(R)

def get_rig_bot(i, j):
    return np.array([corner1[0]+(i+1)*L, corner1[1]+j*L+(L-U)/2]).dot(R)

direction = dict()

for i in range(0, n>>1):
    for j in range(m*i*2, m*(i+1)*2-1):
        if(j%2 != 0):
            direction[j] = "haut" if i%2==0 else "bas"
        elif((j>>1)%2 == 0):
            direction[j] = "droite" if i%2==0 else "gauche"
        else:
            direction[j] = "gauche" if i%2==0 else "droite"


    direction[m*(i+1)*2-1] = "droite"

x = dict()
y = dict()

x[0] = 0
y[0] = 0

for j in range(1, n*m):
    x[j] = x[j-1]
    y[j] = y[j-1]
    if(direction[j-1] == "haut"):
        y[j] += 1
    elif(direction[j-1] == "bas"):
        y[j] -= 1
    elif(direction[j-1] == "gauche"):
        x[j] -= 1
    else:
        x[j] += 1

save = open("pattern.txt", "w+")

# for j in range(0, n*m):
#     save.write("%d %d\n"%(x[j], y[j]))

lineh1 = [] # Top
lineh2 = [] # Bottom
linev1 = [] # Left
linev2 = [] # Right

ver = dict()
hor = dict()
for j in range(0, n*m):
    ver[j] = False
    hor[j] = False

print(n*m)

mission_path = open("mission_path.txt", "w")

for j in range(0,n*m):
    print(j, end=" ")
    if(not ver[j]):
        size = 0
        if(direction[j] == "droite" or direction[j] == "gauche"):
            pt1 = get_top_lef(x[j],y[j])
            pt2 = get_bot_lef(x[j],y[j])
            pt3 = get_top_rig(x[j],y[j])
            pt4 = get_bot_rig(x[j],y[j])
            linev1.append((get_top_lef(x[j],y[j]), get_bot_lef(x[j],y[j])))
            linev2.append((get_top_rig(x[j],y[j]), get_bot_rig(x[j],y[j])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        elif(direction[j] == "bas"):
            while(j+size+1 < n*m and direction[j+size] == "bas"):
                size += 1
            pt1 = get_top_lef(x[j],y[j])
            pt2 = get_bot_lef(x[j],y[j+size])
            pt3 = get_top_rig(x[j],y[j])
            pt4 = get_bot_rig(x[j],y[j+size])
            linev1.append((get_top_lef(x[j],y[j]), get_bot_lef(x[j],y[j+size])))
            linev2.append((get_top_rig(x[j],y[j]), get_bot_rig(x[j],y[j+size])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        elif(direction[j] == "haut"):
            while(j+size+1 < n*m and direction[j+size] == "haut"):
                size += 1
            pt1 = get_bot_lef(x[j],y[j])
            pt2 = get_top_lef(x[j],y[j+size])
            pt3 = get_bot_rig(x[j],y[j])
            pt4 = get_top_rig(x[j],y[j+size])
            linev1.append((get_bot_lef(x[j],y[j]), get_top_lef(x[j],y[j+size])))
            linev2.append((get_bot_rig(x[j],y[j]), get_top_rig(x[j],y[j+size])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        for i in range(j, j+size+1):
            ver[i] = True

    if(not hor[j]):
        size = 0
        if(direction[j] == "bas" or direction[j] == "haut"):
            pt1 = get_lef_top(x[j],y[j])
            pt2 = get_rig_top(x[j],y[j])
            pt3 = get_lef_bot(x[j],y[j])
            pt4 = get_rig_bot(x[j],y[j])
            lineh1.append((get_lef_top(x[j],y[j]), get_rig_top(x[j],y[j])))
            lineh2.append((get_lef_bot(x[j],y[j]), get_rig_bot(x[j],y[j])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        elif(direction[j] == "droite"):
            while(j+size+1 < n*m and direction[j+size] == "droite"):
                size += 1
            print(size)
            pt1 = get_lef_top(x[j],y[j])
            pt2 = get_rig_top(x[j+size],y[j])
            pt3 = get_lef_bot(x[j],y[j])
            pt4 = get_rig_bot(x[j+size],y[j])
            lineh1.append((get_lef_top(x[j],y[j]), get_rig_top(x[j+size],y[j])))
            lineh2.append((get_lef_bot(x[j],y[j]), get_rig_bot(x[j+size],y[j])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        elif(direction[j] == "gauche"):
            while(j+size+1 < n*m and direction[j+size] == "gauche"):
                size += 1
            pt1 = get_rig_top(x[j],y[j])
            pt2 = get_lef_top(x[j+size],y[j])
            pt3 = get_rig_bot(x[j],y[j])
            pt4 = get_lef_bot(x[j+size],y[j])
            lineh1.append((get_rig_top(x[j],y[j]), get_lef_top(x[j+size],y[j])))
            lineh2.append((get_rig_bot(x[j],y[j]), get_lef_bot(x[j+size],y[j])))
            mission_path.write("%f,%f:%f,%f\n"%(pt1[0],pt1[1],pt2[0],pt2[1]))
            mission_path.write("%f,%f:%f,%f\n"%(pt4[0],pt4[1],pt3[0],pt3[1]))
        for i in range(j, j+size+1):
            hor[i] = True

plt.figure(1)

for i in range(0, len(lineh1)):
    c = [random.random(), random.random(), random.random()]
    line = lineh1[i]
    plt.plot([line[0][0], line[1][0]],[line[0][1], line[1][1]], color=c, linewidth=4.0)
    line = lineh2[i]
    plt.plot([line[0][0], line[1][0]],[line[0][1], line[1][1]], color=c, linewidth=4.0)

for i in range(0, len(linev1)):
    c = [random.random(), random.random(), random.random()]
    line = linev1[i]
    plt.plot([line[0][0], line[1][0]],[line[0][1], line[1][1]], color=c, linewidth=4.0)
    line = linev2[i]
    plt.plot([line[0][0], line[1][0]],[line[0][1], line[1][1]], color=c, linewidth=4.0)


plt.show()