import atlas

def cc():
    #a.pond2_2D.rgb_color = [1.0, 1.0, 1.0]
    a.p3 = atlas.Object(id="p3",
                        objtype="object",
                        parents=["lake"],
                        loc="left_meadow",
                        area=[["1.p.4", "1.p.5", "1.p.0"]]
                        )
    #a.fence_2D.rgb_color = [0.5, 0.0, 0.0]


def move():
    a["1"].p[0][0] = -20
    

def aa():
    a.right_meadow_2D.texture_coordinates = [[5.3, 1.8, 0.0], [11.3, 7.8, 0.0]]
    a.detailed_right_meadow.area = [["1.p.13", "1.p.15", "1.p.20"],
                                    ["1.p.14", "1.p.15", "1.p.20"],
                                    ["1.p.14", "1.p.19", "1.p.20"],
                                    ["1.p.14", "1.p.19", "1.p.2"],
                                    ["1.p.18", "1.p.19", "1.p.2"],
                                    ["1.p.18", "1.p.1", "1.p.2"],
                                    ["1.p.18", "1.p.1", "1.p.17"],
                                    ["1.p.16", "1.p.1", "1.p.17"],
                                    ["1.p.16", "1.p.1", "1.p.13"],
                                    ["1.p.16", "1.p.20", "1.p.13"],
                                   ]
