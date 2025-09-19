import argparse
import xml.etree.ElementTree as ET
import sys
from collections import defaultdict
from PIL import Image
import os 
import json

def cropimg(img):
    """
    Crop transparent pixels from an RGBA image.
    Returns the cropped image and the offsets (left, top).
    """
    img = img.convert("RGBA")
    bbox = img.getbbox()  # Returns bounding box of non-zero alpha pixels
    if bbox:
        cropped = img.crop(bbox)
        return cropped, bbox[0], bbox[1]  # x-offset, y-offset
    else:
        return img, 0, 0  # Fully transparent, no crop
    
def areaisempty(atlas, x, y, w, h):
    """Return True if all pixels in the area are fully transparent."""
    if x + w > atlas.width or y + h > atlas.height:
        return False  # Out of bounds
    alpha = atlas.crop((x, y, x + w, y + h)).split()[3]  # Get alpha channel
    return all(a == 0 for a in alpha.getdata())

def main(inputxml, outputdir, maxsize, scale): #scale in %
    basefilename = os.path.basename(inputxml).replace(".xml", "")
    scalefactor = scale/100
    #defs 
    textures = []
    keyframes = []
    indices = defaultdict(list)

    tree = ET.parse(inputxml)
    root = tree.getroot()

    for sub in root.findall("SubTexture"):
        name = sub.get("name")[:-4]  # Remove last 4 chars
        x = int(sub.get("x", 0))
        y = int(sub.get("y", 0))
        w = int(sub.get("width", 0))
        h = int(sub.get("height", 0))
        ox = int(sub.get("frameX", 0))
        oy = int(sub.get("frameY", 0))

        frame = [name, x, y, w, h, ox, oy]

        #make indices
        if frame in keyframes:
            idx = keyframes.index(frame)
        else:
            keyframes.append(frame)
            idx = len(keyframes) - 1
        indices[name].append(idx)

    #now read sprites and pack them
    img = Image.open(inputxml.replace(".xml", ".png")).convert("RGBA")
    os.makedirs(outputdir, exist_ok=True)  
    
    x, y = 0, 0
    atlases = []

    for i, frame in enumerate(keyframes):
        name, fx, fy, fw, fh, ox, oy = frame

        #ensure at least one atlas exists
        if not atlases:
            atlases.append(Image.new("RGBA", (maxsize, maxsize), (0, 0, 0, 0)))
        atlas = atlases[-1]

        sub_img = img.crop((fx, fy, fx + fw, fy + fh))

        #scale the sub-image todo fix ugly rounding
        new_width = int(round(sub_img.width * scalefactor))
        new_height = int(round(sub_img.height * scalefactor))

        sub_img = sub_img.resize((new_width, new_height), Image.Resampling.LANCZOS)
        sub_img, newox, newoy = cropimg(sub_img)

        # Check if image is larger than atlas
        if sub_img.width > atlas.width or sub_img.height > atlas.height:
            raise ValueError("Atlas too small for a single image!")

        # Find the next empty spot, very slow should rewrite
        while not areaisempty(atlas, x, y, sub_img.width, sub_img.height):
            x += 1
            if x + sub_img.width > atlas.width:
                x = 0
                y += sub_img.height;#doesnt work well, should be 1 but 1 is slow
            if y + sub_img.height > atlas.height:
                # Make a new atlas
                atlases.append(Image.new("RGBA", (maxsize, maxsize), (0, 0, 0, 0)))
                atlas = atlases[-1]
                x, y = 0, 0

        texname = basefilename + f"{atlases.index(atlas)}.png"

        # Paste the sub-image
        atlas.paste(sub_img, (x, y), sub_img)
        # update frames
        frame[0] = texname
        frame[1] = x 
        frame[2] = y 
        frame[3] = sub_img.width
        frame[4] = sub_img.height 
        # todo fix ugly rounding
        frame[5] = int(round((ox+newox) * scalefactor))  # ox
        frame[6] = int(round((oy+newoy) * scalefactor))  # oy

        print(f"Placed sprite {i}")

        # Move x for next image
        x += sub_img.width

        if (not texname in textures):
            textures.append(texname)

    for t in textures:
        print(t)

    for i, k in enumerate(keyframes):
        print(i, k)
    for name, values in indices.items():
        print(name, values)
    #save all images
    for i, texname in enumerate(textures):
        atlases[i].save(os.path.join(outputdir, texname))

    #save json

    json_data = {
        "textures": textures,
        "frames": keyframes,
        "indices": [[name, 24, frame_list] for name, frame_list in indices.items()]
    }
    json_path = os.path.join(outputdir, basefilename + ".json")
    with open(json_path, "w") as f:
        json.dump(json_data, f, indent=2)
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_xml")
    parser.add_argument("output_dir")
    parser.add_argument("--maxsize", type=int, default=512)
    parser.add_argument("--scale", type=int, default=40)
    args = parser.parse_args()

    try:
        main(args.input_xml, args.output_dir, args.maxsize, args.scale)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
