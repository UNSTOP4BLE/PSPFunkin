import sys
import os 
import subprocess
import shutil

#add sprites here
sprites = [
    "assets/menu/gftitle/gf.xml",
    "assets/menu/logo.xml",
    "assets/menu/titleenter.xml",
    #characters
    "assets/characters/dad/dad.xml",
]

def excludedirs(files, dirs):
    sprite_dirs = {os.path.dirname(s) for s in dirs}

    filtered_files = [
        f for f in files
        if not any(f.startswith(d + os.sep) or f == d for d in sprite_dirs)
    ]
    return filtered_files

def excludefiles(files, exclude_list):
    filtered_files = [f for f in files if f not in exclude_list]
    return filtered_files

def main(): 
    #find build dirs
    curdir = os.getcwd()
    
    builddirs = [d for d in os.listdir(curdir)
                 if os.path.isdir(d) and d.startswith("build")]
#    print(builddirs)

    #find all assets
    allfiles = [
        os.path.join(root, f)
        for root, dirs, files in os.walk("assets")
        for f in files
    ]

    for s in sprites:
        #exclude specific source files
        allfiles = excludefiles(allfiles, [s, s.replace(".xml", ".png")])

        for b in builddirs:
            dst = os.path.join(b, s.replace(".xml", ".json"))
            if (not os.path.exists(dst) or (os.path.getmtime(s) > os.path.getmtime(dst))):
                print("making asset", dst)
                subprocess.run([sys.executable, "tools/convasset.py", s, os.path.dirname(dst)], text=True)

    #copy remaining files
    for f in allfiles:
        for b in builddirs:
            dst = os.path.join(b, f)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            if (not os.path.exists(dst) or (os.path.getmtime(f) > os.path.getmtime(dst))):
                print("copying file", dst)
                shutil.copy(f, dst)
    
if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
