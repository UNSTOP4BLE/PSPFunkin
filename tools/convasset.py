import xml.etree.ElementTree as ET
from collections import defaultdict
from PIL import Image
import os
import json
import sys

def crop_transparent(img):
    """Crop transparent pixels from an image and return the cropped image and offsets."""
    img = img.convert("RGBA")
    pixels = img.getdata()
    width, height = img.size
    left, top, right, bottom = width, height, 0, 0
    for y in range(height):
        for x in range(width):
            if pixels[y * width + x][3] > 10:  # Non-transparent pixel
                left = min(left, x)
                top = min(top, y)
                right = max(right, x + 1)
                bottom = max(bottom, y + 1)
    
    if left >= right or top >= bottom:
        return img, 0, 0
    
    cropped = img.crop((left, top, right, bottom))
    return cropped, left, top

def scale_image(img, scale_percent=25):
    """Scale an image to 25% of its original size."""
    if scale_percent <= 0:
        raise ValueError("Scale percentage must be positive")
    scale_factor = scale_percent / 100.0
    width, height = img.size
    new_width = max(1, int(width * scale_factor))
    new_height = max(1, int(height * scale_factor))
    return img.resize((new_width, new_height), Image.Resampling.LANCZOS)

def pack_into_atlas(sub_images, frames, texture_name="atlas.png", max_size=512):
    """Pack sub-images into a single 512x512 atlas or raise an error if they don't fit."""
    atlas = Image.new("RGBA", (max_size, max_size), (0, 0, 0, 0))
    x, y, max_row_height = 0, 0, 0
    
    for i, (sub_img, frame) in enumerate(sub_images):
        w, h = sub_img.size
        # Check if image is too large for atlas
        if w > max_size or h > max_size:
            raise ValueError(f"Sub-image {i} with size {w}x{h} exceeds atlas size {max_size}x{max_size}")
        
        # Move to next row if current row is full
        if x + w > max_size:
            x = 0
            y += max_row_height
            max_row_height = 0
        
        # Check if image fits vertically
        if y + h > max_size:
            raise ValueError(f"Sub-image {i} with size {w}x{h} cannot fit in atlas at position ({x}, {y})")
        
        # Paste sub-image into atlas
        atlas.paste(sub_img, (x, y))
        # Update frame coordinates (texture_name, x, y, w, h, offx, offy)
        frames[i][0] = texture_name  # texture_name
        frames[i][1] = x  # x
        frames[i][2] = y  # y
        # Update position
        x += w
        max_row_height = max(max_row_height, h)
    
    return atlas

def main(input_xml_path, output_dir, max_size=512):
    """Process sprite atlas from input XML and texture, scale to 25%, and output to specified directory."""
    # Validate input XML file
    if not os.path.exists(input_xml_path):
        raise FileNotFoundError(f"XML file {input_xml_path} not found")
    
    # Derive texture path from XML path
    img_path = os.path.splitext(input_xml_path)[0] + ".png"
    if not os.path.exists(img_path):
        raise FileNotFoundError(f"Texture file {img_path} not found")
    
    # Load XML
    tree = ET.parse(input_xml_path)
    root = tree.getroot()

    frames = []
    animations = defaultdict(list)

    # Step 1: Parse XML and collect frames
    for sub in root.findall("SubTexture"):
        name = sub.get("name")[:-4]  # Strip last 4 chars
        x = int(sub.get("x", 0))
        y = int(sub.get("y", 0))
        w = int(sub.get("width", 0))
        h = int(sub.get("height", 0))
        offx = int(sub.get("frameX", 0))
        offy = int(sub.get("frameY", 0))

        if w <= 0 or h <= 0:
            print(f"Warning: Skipping invalid frame size {w}x{h} for {name}")
            continue

        # Initialize frame with placeholder for texture_name
        frame_data = ["", x, y, w, h, offx, offy]
        frames.append(frame_data)
        animations[name].append(len(frames) - 1)

    # Step 2: Remove duplicate frames and update indices
    frames_unique = []
    frame_map = {}  # Map from frame tuple to new index
    indices = []

    for anim_name, frame_list in animations.items():
        new_frame_list = []
        for idx in frame_list:
            # Exclude texture_name (index 0) from deduplication
            frame_tuple = tuple(frames[idx][1:])  # Use x, y, w, h, offx, offy
            if frame_tuple in frame_map:
                new_idx = frame_map[frame_tuple]
            else:
                new_idx = len(frames_unique)
                frames_unique.append(frames[idx][:])  # Copy full frame
                frame_map[frame_tuple] = new_idx
            new_frame_list.append(new_idx)
        indices.append([anim_name, 24] + new_frame_list)  # Framerate = 24

    frames = frames_unique

    # Step 3: Load image
    img = Image.open(img_path).convert("RGBA")

    # Step 4: Crop, scale, and pack images
    sub_images = []
    scale_percent = 25  # Fixed 25% scaling
    texture_name = "atlas.png"
    for i, frame in enumerate(frames):
        x, y, w, h, offx, offy = frame[1:]  # Skip texture_name
        if w <= 0 or h <= 0:
            print(f"Warning: Skipping invalid frame size {w}x{h} at index {i}")
            continue
        sub_img = img.crop((x, y, x + w, y + h))
        cropped_img, crop_x, crop_y = crop_transparent(sub_img)
        scaled_img = scale_image(cropped_img, scale_percent)
        # Update offsets and size
        scale_factor = scale_percent / 100.0
        frames[i][5] += int(crop_x * scale_factor)  # offx += scaled crop_x
        frames[i][6] += int(crop_y * scale_factor)  # offy += scaled crop_y
        frames[i][3], frames[i][4] = scaled_img.size  # Update w, h
        sub_images.append((scaled_img, frames[i]))
    
    # Pack into atlas
    try:
        print(f"Packing atlas with scale_percent={scale_percent}%")
        atlas = pack_into_atlas(sub_images, frames, texture_name, max_size)
    except ValueError as e:
        raise ValueError(f"Atlas packing failed at scale_percent={scale_percent}%: {e}")

    # Step 5: Save atlas
    os.makedirs(output_dir, exist_ok=True)
    atlas_path = os.path.join(output_dir, "atlas.png")
    atlas.save(atlas_path)
    print(f"Saved atlas to {atlas_path}")

    # Step 6: Save textures, frames, and indices as JSON
    output_data = {
        "textures": [texture_name],
        "frames": frames,
        "indices": indices
    }
    json_path = os.path.join(output_dir, "atlas.json")
    with open(json_path, "w") as f:
        json.dump(output_data, f, indent=2)
    print(f"Saved JSON to {json_path}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_xml_path> <output_dir>")
        sys.exit(1)
    
    input_xml_path = sys.argv[1]
    output_dir = sys.argv[2]
    
    try:
        main(input_xml_path, output_dir)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)