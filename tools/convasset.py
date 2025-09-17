import xml.etree.ElementTree as ET
from collections import defaultdict
from PIL import Image

# Load XML
path = "assets/menu/options.xml"
tree = ET.parse(path)
root = tree.getroot()

frames = []
animations = defaultdict(list)

# Step 1: parse XML and collect frames
for sub in root.findall("SubTexture"):
    name = sub.get("name")[:-4]  # strip last 4 chars
    x = int(sub.get("x", 0))
    y = int(sub.get("y", 0))
    w = int(sub.get("width", 0))
    h = int(sub.get("height", 0))
    offx = int(sub.get("frameX", 0))
    offy = int(sub.get("frameY", 0))

    frame_data = [x, y, w, h, offx, offy]
    frames.append(frame_data)

    animations[name].append(len(frames) - 1)

# Step 2: remove duplicate frames, update indices
frames_unique = []
frame_map = {}  # map from frame tuple -> new index
indices_new = []

for anim_name, frame_list in animations.items():
    new_frame_list = []
    for idx in frame_list:
        frame_tuple = tuple(frames[idx])
        if frame_tuple in frame_map:
            new_idx = frame_map[frame_tuple]
        else:
            new_idx = len(frames_unique)
            frames_unique.append(list(frame_tuple))
            frame_map[frame_tuple] = new_idx
        new_frame_list.append(new_idx)
    indices_new.append([anim_name, 24] + new_frame_list)  # framerate = 24

# Replace old frames and indices
frames = frames_unique
indices = indices_new

# Step 3: print Python lists
print("frames = [")
for f in frames:
    print("    ", f, ",")
print("]\n")

print("indices = [")
for ind in indices:
    print("    ", ind, ",")
print("]")

# sprite conversion
img = Image.open(path.replace(".xml", ".png"))
for f in frames:
    x, y, w, h = f[:4]
    
    sub_img = img.crop((x,y,x+w,y+h))

    sub_img.show()
    