#!/usr/bin/env python2
#map canvas class and static demo using wxPython on PyOpenGL

#Copyright (C) 2002 by AIR-IX SUUNNITTELU/Ahiplan Oy

#This library is free software; you can redistribute it and/or
#modify it under the terms of the GNU Lesser General Public
#License as published by the Free Software Foundation; either
#version 2.1 of the License, or (at your option) any later version.

#This library is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#Lesser General Public License for more details.

#You should have received a copy of the GNU Lesser General Public
#License along with this library; if not, write to the Free Software
#Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

use_mipmap = 1
draw_triangle_outline = 0
hardware_texture_size = 256

import sys, os, time
sys.path.append("..")
import atlas
from atlas.analyse import fill_attributes
from atlas.geomap import GeoMap
from atlas.media.info import MediaInfo
import string
from copy import deepcopy
from types import *
import Image

from atlas.transport.file import read_file, read_file_as_dict

try:
    from wxPython.wx import *
except ImportError:
    print "Need wxPython from wxPython.org"
    sys.exit(1)

try:
    from wxPython.glcanvas import *
    haveGLCanvas = true
except ImportError:
    haveGLCanvas = false

try:
    # The Python OpenGL package can be found at
    # http://PyOpenGL.sourceforge.net/
    from OpenGL.GL import *
    from OpenGL.GLUT import *
    if use_mipmap:
        import OpenGL.Tk
    haveOpenGL = true
except ImportError:
    haveOpenGL = false

if not haveGLCanvas:
    frame = wxFrame(None, -1, "")
    dlg = wxMessageDialog(frame, 'The wxGLCanvas has not been included with this build of wxPython!',
                          'Sorry', wxOK | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()
    sys.exit(1)
elif not haveOpenGL:
    frame = wxFrame(None, -1, "")
    frame.Show(true)
    print "???"
    dlg = wxMessageDialog(frame,
                          'The OpenGL package was not found.  You can get it at\n'
                          'http://PyOpenGL.sourceforge.net/',
                          'Sorry', wxOK | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()
    sys.exit(1)

try:
    foo = glTexImage2D
    texturing_enabled = 1
except NameError:
    print "You don't have enough recent OpenGL, disabling texturing..."
    texturing_enabled = 0

fonts_enabled = 0
if texturing_enabled:
    try:
        import freetype
        fonts_enabled = 1
        #font_file = "/usr/share/fonts/default/TrueType/timr____.ttf"
        font_file = "/usr/share/fonts/default/TrueType/helr____.ttf"
        #font_file = "/usr/share/fonts/default/TrueType/helcr___.ttf"
        font_bitmaps = {}
    except ImportError:
        print "Fonts disabled"
        print "You need PyFT FreeType Wrappers for Python"
        print "from http://starship.python.net/crew/kernr/Projects.html"
        print "compile line in linux: gcc -O2 -fPIC -shared -I/usr/include/python2.2 -I/usr/include/freetype1/freetype -o freetypecmodule.so freetype_wrap.c -lttf"
        print "install line: cp freetypecmodule.so freetype.py /usr/lib/python2.2/site-packages/"
        print "replace 2.2 with your python version"
else:
    print "Fonts disabled, because texturing disabled"
    

if not (texturing_enabled and fonts_enabled):
    raw_input("press enter to continue...")


def drawCharacter(char, font=font_file, size=24, type='pix'):
    e = freetype.TT_Init_FreeType()

    if type=='pix':
        freetype.TT_Set_Raster_Gray_Palette(e, (0, 64, 128, 192, 255))
        #freetype.TT_Set_Raster_Gray_Palette(e, (0, 0, 255, 255, 255))

    face = freetype.TT_Open_Face(e, font)

    props = freetype.TT_Get_Face_Properties(face)

    ins = freetype.TT_New_Instance(face)
    freetype.TT_Set_Instance_Resolutions(ins, 96, 96)
    freetype.TT_Set_Instance_PointSize(ins, size)

    for id in range(props.num_CharMaps):
        tup = freetype.TT_Get_CharMap_ID(face, id)
        if tup == (3,1):
            # Windows Unicode mapping
            cm = freetype.TT_Get_CharMap(face, id)
            break
        
    glyphIndex = freetype.PyTT_Char_Index(cm, char)

    glyph = freetype.TT_New_Glyph(face)
    freetype.TT_Load_Glyph(ins, glyph, glyphIndex, freetype.TTLOAD_DEFAULT)
    metric = freetype.TT_Get_Glyph_Metrics(glyph)
    #print "bbox:", metric.bbox.xMin, metric.bbox.yMin, metric.bbox.xMax, metric.bbox.yMax
    #print "bearingX :", metric.bearingX 
    #print "bearingY :", metric.bearingY 
    #print "advance :", metric.advance 

    #width = metric.advance / 64
    width = (metric.bbox.xMax - metric.bbox.xMin) / 64

    height = (metric.bbox.yMax - metric.bbox.yMin) / 64

    raster = freetype.TT_Raster_Map(width, height, type)

    curX = 0

    if type=='bit':
        freetype.TT_Get_Glyph_Bitmap(glyph, raster, -metric.bbox.xMin, -metric.bbox.yMin)
    else:
        freetype.TT_Get_Glyph_Pixmap(glyph, raster, -metric.bbox.xMin, -metric.bbox.yMin)

    raster.metric = metric

    return raster

def rot2glmatrix(rot):
##    print rot
##    print map(lambda row:row + [0.0], rot) + [0.0, 0.0, 0.0, 1.0]
##    print "-"*60
    return map(lambda row:row + [0.0], rot) + [0.0, 0.0, 0.0, 1.0]

class MapCanvas:
    def init_GL(self):
        glClearColor(0.5, 0.5, 0.5, 1.0)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        glBindTexture(GL_TEXTURE_2D, glGenTextures(1))
        glPixelStorei(GL_UNPACK_ALIGNMENT,1)
        #glTexImage2D(GL_TEXTURE_2D, 0, 4, 512, 512, 0,
        #             GL_RGBA, GL_UNSIGNED_BYTE, img.tostring)
        #glTexImage2D(GL_TEXTURE_2D, 0, 3, img.size[0], img.size[1], 0,
        #             GL_RGBA, GL_UNSIGNED_BYTE, img.tostring("raw", "RGBA", 0, -1))
        glEnable(GL_TEXTURE_2D)
        #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)
        #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL)
        #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND)
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)
        #glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (0.0, 0.0, 0.0, 0.0))
        #glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (0.0, 0.0, 0.0, 1.0))
        #glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (1.0, 1.0, 1.0, 0.0))
        #glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (1.0, 1.0, 1.0, 1.0))
        if not use_mipmap:
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)
        
        #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)
        #glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)
        #glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR)
        #glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP)
        #glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP)

        self.font_texture_info = {}
        self.top_object = None
        self.last_texture = None
        self.special_stuff = []

    def view_all(self, objects):
        pass

    def set_2D_view(self, minx, miny, maxx, maxy):
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        glOrtho(minx, maxx, miny, maxy, -1, 1)
        self.screen_limits = minx, maxx, miny, maxy
        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()

    def check_class(self, obj):
        if self.map.is_geo_object(obj):
            return self.map.convert2geo(obj)
        return obj

    def apply_operation(self, obj, op):
        res = []
        media_method = getattr(self.media_info, op+"_object")
        map_method = getattr(self.map, op+"_object")
        if obj.has_parent("root_media_info"):
            res = media_method(obj)
        elif obj.has_parent("root_geometry"):
            res = map_method(obj)
        else:
            res = media_method(obj)
            if res:
                res = res + map_method(obj)
            else:
                res = map_method(obj)
        if not res: res = []
        self.OnPaint(None)
        return res

    def create_object(self, obj):
        return self.apply_operation(obj, "create")

    def set_object(self, obj):
        return self.apply_operation(obj, "set")

    def delete_object(self, obj):
        return self.apply_operation(obj, "delete")

    def draw_object(self, obj, depth, count=0):
        redraw_flag = 0
        if not obj: return
        if not self.map.objects.has_key(obj.id): return
        if id(obj)!=id(self.map.objects[obj.id]):
            print obj.id, "not same id()!!"
        if not hasattr(obj, "detailed_contents"):
            print "????detailed_contents?",
            atlas.print_parents(obj)
            #print "self.map.objects:",
            #atlas.print_parents(self.map.objects[obj.parents[0]])
            #import pdb; pdb.set_trace()
        if not obj.detailed_contents:
            if obj._polyline:

##                mi = self.media_info.lookup(obj, "2D")
##                if mi:
##                    apply(glColor3f, mi.rgb_color)
##                    glLineWidth(mi.line_width)
##                else:
##                    glColor3f(0.0, 0.0, 0.0)
##                    glLineWidth(1.0)
##                for i in range(len(obj._polyline)-1):
##                    p1, p2 = obj._polyline[i:i+2]
##                    glPushMatrix()
##                    glTranslated(p1.x, p1.y, 0.0)
##                    glBegin(GL_LINES)
##                    glVertex3f(0.0, 0.0, depth)
##                    glVertex3f(p2.x - p1.x, p2.y - p1.y, depth)
##                    glEnd()
##                    glPopMatrix()
##                glLineWidth(1.0)

                glPushMatrix()
                p = atlas.Object()
                p.x = self.middle_x
                p.y = self.middle_y
                #p = obj._polyline[0]
                glTranslated(p.x, p.y, 0.0)
                mi = self.media_info.lookup(obj, "2D")
                if mi:
                    apply(glColor3f, mi.rgb_color)
                    glLineWidth(mi.line_width)
                else:
                    glColor3f(0.0, 0.0, 0.0)
                    glLineWidth(1.0)
                glBegin(GL_LINE_STRIP)
                for v in obj._polyline:
                    glVertex3f(v.x - p.x, v.y - p.y, depth)
                glEnd()
                glLineWidth(1.0)
                glPopMatrix()

            if obj._area:
                use_texture = 0
                special_flag = 0
                mi = self.media_info.lookup(obj, "2D")
                if mi:
                    if texturing_enabled and hasattr(mi, "texture_coordinates"):
                        if type(mi.texture_source)==StringType:
                            art_info = self.media_info[mi.texture_source]
                        else:
                            art_info = mi.texture_source
                        if art_info.data_uri[:5]=="file:":
                            file_name = art_info.data_uri[5:]
                        else:
                            raise ValueError, "url type not supported (yet): "+art_info.data_uri
                        use_texture = 1
                        if file_name[-4:]==".pnm":
                            fp = open(file_name)
                            fp.readline(); fp.readline()
                            width,height = map(int, string.split(fp.readline()))
                            special_flag = max(width, height)>256
                            if not special_flag: fp.close()
                        if special_flag:
                            print "special!"
                            special_flag = 1
                            fp.readline()
                            self.special_stuff.append(atlas.Object(texture = fp.read(),
                                                                   width = width,
                                                                   height = height,
                                                                   coordinates = mi.texture_coordinates))
                            fp.close()
                        else:
                            img = Image.open(file_name)
                            img = img.convert("RGBA")
                            print img.size
                            texture = img.tostring("raw", "RGBA", 0, -1)
                            if texture!=self.last_texture:
                                if use_mipmap:
                                    OpenGL.Tk.gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img.size[0], img.size[1],
                                                                GL_RGBA, GL_UNSIGNED_BYTE, texture)
                                else:
                                    glTexImage2D(GL_TEXTURE_2D, 0, 3, img.size[0], img.size[1], 0,
                                                 GL_RGBA, GL_UNSIGNED_BYTE, texture)
                                self.last_texture = texture

                            redraw_flag = 1

    ##                        if count:
    ##                            rasterMap = freetype.drawString("a", font=font_file, size=444, type='pix')
    ##                            bitmap = rasterMap.tostring()
    ##                            bitmap2 = string.join(map(lambda c:chr(255)*3+c, bitmap), "")
    ##                            #bitmap2 = string.join(map(lambda c:"\0\0\0" + c, bitmap), "")
    ##                            #bitmap2 = string.join(map(lambda c:c+c+c+"\0", bitmap), "")
    ##                            #bitmap2 = string.join(map(lambda c:c*4, bitmap), "")
    ##                            print rasterMap.width, rasterMap.rows, len(bitmap), len(bitmap2)
    ##                            #img = freetype.toImage(rasterMap, type='pix')
    ##                            #img.save("a.png")

    ##                            img = Image.open("a_rgb.png")
    ##                            #58 68
    ##                            #glTexImage2D(GL_TEXTURE_2D, 0, 1, rasterMap.width, rasterMap.rows, 0,
    ##                            #glTexImage2D(GL_TEXTURE_2D, 0, 2, 256, 256, 0,
    ##                            #             GL_COLOR_INDEX, GL_BITMAP, bitmap)
    ##                            glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, 256, 256, 0,
    ##                                         GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, bitmap*3)
    ##                            #glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0,
    ##                            #             GL_RGBA, GL_UNSIGNED_BYTE, bitmap2)

                        
                            tx1, ty1, tz1 = mi.texture_coordinates[0]
                            tx2, ty2, tz2 = mi.texture_coordinates[1]
                            #glColor3f(1.0, 1.0, 0.0)
                            #glColor3f(0.0, 0.0, 0.0)
                            #glColor3f(1.0, 1.0, 1.0)
                    else:
                        try:
                            apply(glColor3f, mi.rgb_color)
                        except:
                            import pdb; pdb.set_trace()
                else:
                    glColor3f(0.0, 0.0, 0.0)

                if not special_flag:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)

                    #glTexImage2D(GL_TEXTURE_2D, 0, 4, 512, 512, 0,
                    #             GL_RGBA, GL_UNSIGNED_BYTE, img.tostring)
                    #glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0,
                    #             GL_RGB, GL_UNSIGNED_BYTE, img.tostring)
                    #use_texture = obj.id=="right_meadow"
                    #use_texture = obj.id in ["left_meadow", "right_meadow"]
                    if use_texture:
                        #print "?"
                        glEnable(GL_TEXTURE_2D)
    ##                if count and fonts_enabled:
    ##                    glEnable(GL_ALPHA_TEST)
    ##                    glEnable(GL_BLEND)
    ##                    glAlphaFunc(GL_GEQUAL, 0.0625)
    ##                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    ##                    #glEnable(GL_POLYGON_OFFSET)
    ##                    #glPolygonOffset(0.0, -3)
    ##                    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)
    ##                    #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL)
    ##                    #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND)
    ##                    #glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)
    ##                    glColor3f(1.0, 0.0, 0.0)
                    glBegin(GL_TRIANGLES)
                    for t in obj._area:
                        for v in t:
                            if use_texture:
                                #print "?"*10
                                #print v.x, v.y
                                #print tx1, tx2-tx1, ty1, ty2-ty1
                                #print (v.x-tx1)/(tx2-tx1), (v.y-ty1)/(ty2-ty1)
                                glTexCoord2d((v.x-tx1)/(tx2-tx1), (v.y-ty1)/(ty2-ty1))
                            glVertex3f(v.x, v.y, depth)
                    glEnd()
                    if use_texture:
                        glDisable(GL_TEXTURE_2D)
    ##                if fonts_enabled:
    ##                    glDisable(GL_ALPHA_TEST)
    ##                    glDisable(GL_BLEND)
    ##                    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)
                glColor3f(0.0, 0.0, 0.0)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                if draw_triangle_outline:
                    glBegin(GL_TRIANGLES)
                    for t in obj._area:
                        for v in t:
                            glVertex3f(v.x, v.y, depth)
                    glEnd()
                    
            if obj.get("text") and fonts_enabled:
                #print obj.text
                mi = self.media_info.lookup(obj, "2D")
                if mi:
                    apply(glColor3f, mi.rgb_color)
                    #print "with color:", mi.rgb_color
                else:
                    glColor3f(0.0, 0.0, 0.0)
                texture_size = hardware_texture_size
                if not self.font_texture_info:
                    texture = ["\0" * texture_size] * texture_size
                    curX = 0
                    curY = 0
                    curHeight = 0
                    extra_pixels = 1

                    bitmap_seen = {}
                    for char in map(chr, range(256)):
                    #for char in 'V\xc4 )(+-,/.1032547698?ACBEDFIHKJMLONPSRUT\xd6YX':
                        #render font
                        #print char, ord(char)
                        #rasterMap = drawCharacter(char, size=17)
                        rasterMap = drawCharacter(char, size=19)
                        #rasterMap = drawCharacter(char, size=41)
                        bitmap = rasterMap.tostring()
                        #img = freetype.toImage(rasterMap, 'pix')
                        #img.save("test.png")
                        #advance line if needed (and check limits)
                        while 1:
                            if rasterMap.cols + curX <= texture_size and \
                               rasterMap.rows + curY <= texture_size: break
                            if rasterMap.cols + curX > texture_size:
                                if curX:
                                    curX = 0
                                    curY += curHeight + extra_pixels
                                else:
                                    raise ValueError, "too big size for fonts"
                            if rasterMap.rows + curY > texture_size:
                                raise ValueError, "too big size for fonts"
                        #bitmap seen?
                        key = rasterMap.cols, rasterMap.rows, bitmap
                        if key in bitmap_seen:
                            self.font_texture_info[char] = bitmap_seen[key]
                            continue
                        bitmap_seen[key] = char

                        #store info about glyph
                        tbbox = atlas.Object(xmin = float(curX) / texture_size,
                                             ymin = float(curY) / texture_size,
                                             xmax = float(curX + rasterMap.cols) / texture_size,
                                             ymax = float(curY + rasterMap.rows) / texture_size)
                        fbbox = atlas.Object(xmin = rasterMap.metric.bbox.xMin,
                                             ymin = rasterMap.metric.bbox.yMin,
                                             xmax = rasterMap.metric.bbox.xMax,
                                             ymax = rasterMap.metric.bbox.yMax)
                        self.font_texture_info[char] = atlas.Object(
                            texture_bbox = tbbox,
                            font_bbox = fbbox,
                            advance = rasterMap.metric.advance)

                        #copy font into texture
                        y = curY + rasterMap.rows
                        for pos in range(0, len(bitmap), max(1, rasterMap.cols)):
                            y -= 1
                            texture[y] = texture[y][:curX] + \
                                         bitmap[pos:pos+rasterMap.cols] + \
                                         texture[y][curX+rasterMap.cols:]

##                        y = curY
##                        for pos in range(0, len(bitmap), max(1, rasterMap.cols)):
##                            y += 1
##                            texture[y] = texture[y][:curX] + \
##                                         bitmap[pos:pos+rasterMap.cols] + \
##                                         texture[y][curX+rasterMap.cols:]
                        
                        curX += rasterMap.cols + extra_pixels
                        curHeight = max(curHeight, rasterMap.rows)
                        #print rasterMap.width, rasterMap.cols, rasterMap.rows, len(bitmap)
                        #print curX,curY, curHeight
                    #open("foo2.out", "w").write(repr(bitmap)) 
                   #open("foo.out", "w").write(repr(texture))
                    self.font_texture = string.join(texture, "")
                    #self.font_texture = string.join(map(lambda c:c*2, self.font_texture), "")
                    im = Image.fromstring("L", (texture_size, texture_size),
                                          self.font_texture, "raw", "L;I")
                    im.save("font_texture.png")


                if self.font_texture!=self.last_texture:
                    #glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, 256, 256, 0,
                    #             GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texture*4)
                    if use_mipmap:
                        #OpenGL.Tk.gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY4, texture_size, texture_size,
                        #                            GL_LUMINANCE, GL_UNSIGNED_BYTE, self.font_texture)
                        OpenGL.Tk.gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY, texture_size, texture_size,
                                                    GL_LUMINANCE, GL_UNSIGNED_BYTE, self.font_texture)
                        #OpenGL.Tk.gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY, texture_size, texture_size,
                        #                            GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, self.font_texture)
                    else:
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY, texture_size, texture_size, 0,
                                     GL_LUMINANCE, GL_UNSIGNED_BYTE, self.font_texture)
                    self.last_texture = self.font_texture

                glEnable(GL_TEXTURE_2D)
                glEnable(GL_ALPHA_TEST)
                glEnable(GL_BLEND)
                glAlphaFunc(GL_GEQUAL, 0.0625)
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
                #glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA)
                #glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE)
                #glEnable(GL_POLYGON_OFFSET)
                #glPolygonOffset(0.0, -3)
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)


                glPushMatrix()
                glTranslated(self.middle_x, self.middle_y, 0.0)
                p = obj._pos
                glTranslated(p.x - self.middle_x, p.y - self.middle_y, 0.0)
                glMultMatrixd(rot2glmatrix(obj.rotation))
                x, y = 0.0, 0.0
                glBegin(GL_QUADS)
                if 0: #test...
                    x1 = x
                    y1 = y
                    x2 = x + 10.0
                    y2 = y + 10.0
                    glTexCoord2d(0.0, 0.0)
                    glVertex3f(x1, y1, depth)
                    glTexCoord2d(1.0, 0.0)
                    glVertex3f(x2, y1, depth)
                    glTexCoord2d(1.0, 1.0)
                    glVertex3f(x2, y2, depth)
                    glTexCoord2d(0.0, 1.0)
                    glVertex3f(x1, y2, depth)
                else:
                    fp = open("debug.log", "a+")
                    fp.write(obj.text+"\n")
                    for char in obj.text:
                        ti = self.font_texture_info[char]
                        size = 1./600
                        x1 = x + ti.font_bbox.xmin * size
                        y1 = y + ti.font_bbox.ymin * size
                        x2 = x + ti.font_bbox.xmax * size
                        y2 = y + ti.font_bbox.ymax * size
                        #print char, ord(char), ti
                        #print char, ord(char), x1,y1,x2,y2, ti.texture_bbox, x
                        fp.write(string.join(map(str, [char, ord(char), x, y, x1, y1, x2, y2, ti, ti.advance * size])) + "\n")
                        glTexCoord2d(ti.texture_bbox.xmin, ti.texture_bbox.ymin)
                        glVertex3f(x1, y1, depth)
                        glTexCoord2d(ti.texture_bbox.xmax, ti.texture_bbox.ymin)
                        glVertex3f(x2, y1, depth)
                        glTexCoord2d(ti.texture_bbox.xmax, ti.texture_bbox.ymax)
                        glVertex3f(x2, y2, depth)
                        glTexCoord2d(ti.texture_bbox.xmin, ti.texture_bbox.ymax)
                        glVertex3f(x1, y2, depth)
                        x += ti.advance * size
                    fp.close()
##                tex_size = 1.0
##                glTexCoord2d(0, 0)
##                glVertex3f(p.x, p.y, depth)
##                glTexCoord2d(tex_size, 0)
##                glVertex3f(p.x+1, p.y, depth)
##                glTexCoord2d(tex_size, tex_size)
##                glVertex3f(p.x+1, p.y+1, depth)
##                glTexCoord2d(0, tex_size)
##                glVertex3f(p.x, p.y+1, depth)
                glEnd()
                glPopMatrix()

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)


                glDisable(GL_TEXTURE_2D)
                glDisable(GL_ALPHA_TEST)
                glDisable(GL_BLEND)
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)


        for sub_obj in obj.contains:
            #print "unhashable?", type(sub_obj.id), "for:", obj.id
            #if self.map.objects.has_key(sub_obj.id) and id(sub_obj)!=id(self.map.objects[sub_obj.id]):
            #    print obj.id, sub_obj.id, "obj/sub: not same id()!!"
            #    print obj.__class__, sub_obj.__class__
            self.draw_object(sub_obj, depth)

##        if not count and redraw_flag:
##            self.draw_object(obj, depth, count+1)

        obj._changed = 0


    def draw_map(self, gmap):
        # clear color and depth buffers
        if self.top_object:
            top_object = self.top_object
        else:
            top_object = None
            for obj in gmap.objects.values():
                if obj.objtype=="object" and not obj.loc and not top_object and obj.contains:
                    top_object = obj
                    if not top_object.has_parent("world"):
                        print "top_object is not inherited from world!"
                        print top_object.id
            self.top_object = top_object
        #print "top_object:", top_object
        
        if not hasattr(top_object, "_changed") or top_object._changed:
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
            self.map_dl = glGenLists(1)
            glNewList(self.map_dl, GL_COMPILE_AND_EXECUTE)
            glColor3f(0.0, 0.0, 0.0)
            self.draw_object(top_object, 0.0)
            glEndList()

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        for special in self.special_stuff:
            print special.coordinates
            from atlas.util import texture
            #d?l[xy][12]: screen limits in map world
            lx1, lx2, ly2, ly1 = self.screen_limits
            dlx = lx2 - lx1
            dly = ly2 - ly1
            #s[xy][12]: screen coordinates
            for sx1 in range(0, self.screen_size[0], hardware_texture_size):
                sx2 = sx1+hardware_texture_size
                #[xy][12]: portion of screen in map world
                x1 = lx1 + dlx * sx1 / self.screen_size[0]
                x2 = lx1 + dlx * sx2 / self.screen_size[0]
                for sy1 in range(0, self.screen_size[1], hardware_texture_size):
                    sy2 = sy1+hardware_texture_size
                    y1 = ly1 + dly * sy1 / self.screen_size[1]
                    y2 = ly1 + dly * sy2 / self.screen_size[1]
                    
                    tx1, ty1, tz1 = special.coordinates[0]
                    tx2, ty2, tz2 = special.coordinates[1]
                    dx = tx2 - tx1
                    dy = ty2 - ty1
                    t2x1 = (x1 - tx1) / dx * special.width
                    t2x2 = (x2 - tx1) / dx * special.width
                    t2y1 = (ty2 - y1) / dy * special.height
                    t2y2 = (ty2 - y2) / dy * special.height
                    texture_data = texture.sample(special.texture,
                                                  special.width, special.height,
                                                  hardware_texture_size, hardware_texture_size,
                                                  t2x1, t2y1, t2x2, t2y2)
                    OpenGL.Tk.gluBuild2DMipmaps(GL_TEXTURE_2D, 3, hardware_texture_size, hardware_texture_size,
                                                GL_RGB, GL_UNSIGNED_BYTE, texture_data)
                    #glTexImage2D(GL_TEXTURE_2D, 0, 3, hardware_texture_size, hardware_texture_size, 0,
                    #             GL_RGB, GL_UNSIGNED_BYTE,
                    #             texture_data)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
                    glEnable(GL_TEXTURE_2D)
                    glBegin(GL_QUADS)
                    #print "???", len(texture_data), x1, x2, y1, y2, sx1,sx2,sy1,sy2
                    #fp = open("foo.pnm", "w")
                    #fp.write("P6\n# Debug\n100 100\n255\n")
                    #fp.write(texture_data)
                    #fp.close()

                    glTexCoord2d(0.0,0.0); glVertex2d(x1,y1)
                    glTexCoord2d(1.0,0.0); glVertex2d(x2,y1)
                    glTexCoord2d(1.0,1.0); glVertex2d(x2,y2)
                    glTexCoord2d(0.0,1.0); glVertex2d(x1,y2)
                    glEnd()
                    glDisable(GL_TEXTURE_2D)
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
        glCallList(self.map_dl)
            
            


class wxMapCanvas(wxGLCanvas,MapCanvas):
    def __init__(self, parent, map, media_info):
        wxGLCanvas.__init__(self, parent, -1)
        self.init = false
        self.map = map
        self.media_info = media_info
        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        EVT_SIZE(self, self.OnSize)
        EVT_PAINT(self, self.OnPaint)
        EVT_LEFT_UP(self, self.OnLeftMouseUp)
        EVT_RIGHT_UP(self, self.OnRightMouseUp)
        EVT_MOTION(self, self.OnMouseMotion)
        EVT_KEY_DOWN(self, self.OnKeyDown)
        #self.last_size = None
        self.middle = 0.5, 0.5
        self.zoom = 1.1
        #self.zoom = 0.1

    def OnEraseBackground(self, event):
        pass # Do nothing, to avoid flashing on MSW.

    def OnSize(self, event):
        size = self.GetClientSize()
        if self.GetContext() and size.width>0 and size.height>0:
            self.SetCurrent()
            glViewport(0, 0, size.width, size.height)
            self.screen_size = size.width, size.height
            self.check_size()

    def get_relative_pos(self, event):
        size = self.GetClientSize()
        pos = event.GetPosition()
        posx = pos.x / float(size.width)
        posy = (size.height - pos.y - 1) / float(size.height)
        print size, pos, posx, posy
        return posx, posy

    def OnLeftMouseUp(self, event):
        print "left",
        posx, posy = self.get_relative_pos(event)
        self.middle = self.middle[0] + (posx-0.5) * self.zoom, \
                      self.middle[1] + (posy-0.5) * self.zoom,
        self.zoom = self.zoom / 2.0
        self.OnPaint(event)

    def OnRightMouseUp(self, event):
        print "right",
        posx, posy = self.get_relative_pos(event)
        self.middle = self.middle[0] + (posx-0.5) * self.zoom, \
                      self.middle[1] + (posy-0.5) * self.zoom,
        self.zoom = self.zoom * 2.0
        self.OnPaint(event)

    def OnMouseMotion(self, event):
        event.Skip()

    def OnKeyDown(self, event):
        print event, event.KeyCode(), time.time()
        if event.KeyCode()==ord("+"):
            self.zoom = self.zoom * 0.9
            self.OnPaint(event)
        elif event.KeyCode()==ord("-"):
            self.zoom = self.zoom * 1.1
            self.OnPaint(event)
        elif event.KeyCode()==ord("="):
            self.middle = 0.5, 0.5
            self.zoom = 1.1
            self.OnPaint(event)
        else:
            event.Skip()

    def check_size(self):
        size = self.GetClientSize()
        size_width = float(size.width)
        size_height = float(size.height)
        self.screen_size = size_width, size_height
        print "-"*60
        print len(self.map.objects)
        print size_width, size_height
        if not self.top_object: # or self.last_size!=(size_width, size_height):
            xlimits, ylimits, zlimits = self.map.dimensions()
            if xlimits.min==None:
                xlimits.add(0.0); xlimits.add(1.0)
                ylimits.add(0.0); ylimits.add(1.0)
            if xlimits.size()==0.0:
                xlimits.add(xlimits.min + 1.0)
            if ylimits.size()==0.0:
                ylimits.add(ylimits.min + 1.0)
            self.limits = xlimits, ylimits, zlimits
            #self.last_size = size_width, size_height
        xlimits, ylimits, zlimits = deepcopy(self.limits)

        #xlimits.extend(percentage=10.0)
        #ylimits.extend(percentage=10.0)
##        size_x = map_maxx - map_minx
##        size_y = map_maxy - map_miny
##        border_ratio = 20.0
##        map_minx -= size_x / border_ratio
##        map_maxx += size_x / border_ratio
##        size_x += size_x / border_ratio * 2.0
##        map_miny -= size_y / border_ratio
##        map_maxy += size_y / border_ratio
##        size_y += size_y / border_ratio * 2.0
        #if size.width > size.height:
        print xlimits, ylimits #map_minx, map_miny, map_maxx, map_maxy
        print xlimits.size(), ylimits.size()
        print size_width/size_height, xlimits.size()/ylimits.size()
        if size_width/size_height > xlimits.size()/ylimits.size():
            print "x extended"
            ratio = size_height / ylimits.size()
            xlimits.extend(size_width/ratio)
        else:
            print "y extended"
            ratio = size_width / xlimits.size()
            ylimits.extend(size_height/ratio)
        xlimits.set_middle_relative(self.middle[0])
        ylimits.set_middle_relative(self.middle[1])
        xlimits.zoom(self.zoom)
        ylimits.zoom(self.zoom)
        self.middle_x = xlimits.middle()
        self.middle_y = ylimits.middle()
        minx, maxx = xlimits.min, xlimits.max
        miny, maxy = ylimits.min, ylimits.max
        print minx,miny, maxx,maxy
        print "zoom position:", self.middle, self.zoom
        self.set_2D_view(minx,miny, maxx,maxy)

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.SetCurrent()
        if not self.init:
            self.init_GL()
            self.init = true
        self.check_size()
        self.draw_map(self.map)
        self.SwapBuffers()


class MapApp(wxApp):
    def __init__(self, name, map, media_info):
        self.name = name
        self.map = map
        self.media_info = media_info
        wxApp.__init__(self, 0)

    def OnInit(self):
        wxInitAllImageHandlers()
        frame = wxFrame(None, -1, self.name, size=(0,0),
                        style=wxNO_FULL_REPAINT_ON_RESIZE|wxDEFAULT_FRAME_STYLE)
        frame.CreateStatusBar()
        map_canvas = wxMapCanvas(frame, self.map, self.media_info)
        menuBar = wxMenuBar()
        menu = wxMenu()
        menu.Append(101, "E&xit\tAlt-X", "Exit demo")
        EVT_MENU(self, 101, self.OnButton)
        menuBar.Append(menu, "&File")
        frame.SetMenuBar(menuBar)
        frame.Show(true)

        # so set the frame to a good size for showing stuff
        frame.SetSize((640, 640))
        #frame.SetSize((256, 256))
        frame.SetPosition((100,100))

        self.SetTopWindow(frame)
        self.frame = frame

        return true

    def OnButton(self, evt):
        self.frame.Close(true)

def combine_files(files, prefix=""):
    dict = {}
    for file in files:
        if file[:5]=="file:":
            file = file[5:]
        else:
            raise ValueError, "uri type not supported: " + file
        print file
        dict.update(read_file_as_dict(prefix + file))
    fill_attributes(dict.values())
    return dict

def file_as_operations(dict, file):
    for op in read_file(file):
        if op.parents[0]=="create":
            obj = op.arg
            #dict[obj.loc].contains.append(obj.id)
            dict[obj.loc].contains.insert(0, obj.id)
            dict[obj.id] = obj
            for i in range(len(obj.parents)):
                obj.parents[i] = dict[obj.parents[i]]
            print obj
            #import pdb; pdb.set_trace()

def load_map_and_media_as_atlas_objects(makefile):
    prefix, file = os.path.split(makefile)
    if prefix: prefix = prefix + os.sep
    sources = read_file(makefile)[0]

    all_objects = combine_files(sources.world_sources, prefix)
    #CHEAT: does only this special case ...
    if hasattr(sources, "world_sources_operations") and sources.world_sources_operations:
        file_as_operations(all_objects, sources.world_sources_operations[0][5:])
    
    all_media_info_dict = combine_files(sources.media_sources, prefix)

    return all_objects, all_media_info_dict

    base_definitions = read_file_as_dict("../../../protocols/atlas/spec/atlas.atlas")
    class_definitions = read_file_as_dict("../examples/meadow_class.atlas")
    objects = read_file_as_dict("../examples/meadow.atlas")
    all_objects = {}
    all_objects.update(base_definitions)
    all_objects.update(class_definitions)
    all_objects.update(objects)
    fill_attributes(all_objects.values())
    
    media_info_dict = read_file_as_dict("../examples/meadow_media.atlas")
    all_media_info_dict = {}
    all_media_info_dict.update(base_definitions)
    all_media_info_dict.update(media_info_dict)
    fill_attributes(all_media_info_dict.values())
    return all_objects, all_media_info_dict
    

def load_map_and_media(makefile):
    all_objects, all_media_info_dict = load_map_and_media_as_atlas_objects(makefile)
    #img = Image.open("../examples/right_meadow.png")
    #s = img.tostring()
    #print img.__dict__.keys()
    #print len(s), img.mode, img.size
    #print objects
    #print "-"*60
    #analyse_geometry(objects)
    gmap = GeoMap(all_objects)
    media_info = MediaInfo(all_media_info_dict)
    #print objects
    return gmap, media_info


if __name__ == "__main__":
    if len(sys.argv)<=1:
        gmap, media_info = load_map_and_media("../examples/meadow_makefile.atlas")
    else:
        gmap, media_info = load_map_and_media(sys.argv[1])
    app = MapApp("MapCanvas", gmap, media_info)
    app.MainLoop()
