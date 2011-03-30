#!/usr/bin/env python
from map_canvas import *
all_objects, all_media_info_dict = load_map_and_media_as_atlas_objects()
all_objects.update(all_media_info_dict)
fill_attributes(all_objects.values())
from atlas.transport.file import write_file
write_file(all_objects.values(), "simple_core.atlas")
