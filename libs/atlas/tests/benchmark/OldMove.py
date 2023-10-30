import sys
sys.path.append("../../../protocols/atlas/libatlas/libAtlasPy")
from timer import *
from atlas import *

class NPC:
    def __init__(self):
        self.id = "123"
        self.x = 0.0
        self.y = 0.0
        self.z = 0.0
        self.vx = 0.0
        self.vy = 0.0
        self.vz = 0.0
    def move(self, op):
        self.vx, self.vy, self.vz = op[0].location.velocity
        self.x = self.x + self.vx
        self.y = self.y + self.vy
        self.z = self.z + self.vz
        
        loc = Location(coordinates=[self.x, self.y, self.z],
                       velocity=[self.vx, self.vy, self.vz])
        human = Entity(self.id, location=loc)
        move = Operation("move", human)
        return Operation("sight", move, from_=self.id)

time_on()
maxi = 10000
for i in range(maxi):
    loc = Location(coordinates=[i+0.0, i-1.0, i+1.0],
                   velocity=[i+0.0, i-1.0, i+1.0])
    human = Entity(type=["human"], location=loc)
    move = Operation("move", human)
    sight = Operation("sight", move, from_="123")
time_off("Plain creating of sight operation", maxi)

npc1 = NPC()
time_on()
for i in range(maxi):
    loc = Location(velocity=[i+0.0, i-1.0, i+1.0])
    human = Entity(npc1.id, location=loc)
    move = Operation("move", human)
    res_sight = npc1.move(move)
    x,y,z = res_sight[0][0].location.coordinates
time_off("NPC movements", maxi)
print "Resulting position: (%s,%s,%s)" % (x,y,z)
