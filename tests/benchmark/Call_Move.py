from timer import *

class NPC:
    def __init__(self):
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        self.vx = 0.0
        self.vy = 0.0
        self.vz = 0.0
    def move(self, vx, vy, vz):
        self.vx, self.vy, self.vz = vx, vy, vz
        self.x = self.x + self.vx
        self.y = self.y + self.vy
        self.z = self.z + self.vz
        return self.x, self.y, self.z

maxi=100000
npc1 = NPC()
time_on()
for i in range(maxi):
    x,y,z = npc1.move(i+0.0, i-1.0, i+1.0)
time_off("NPC movements", maxi)
print "Resulting position: (%s,%s,%s)" % (x,y,z)
