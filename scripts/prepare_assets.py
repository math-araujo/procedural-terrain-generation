import argparse
import pathlib
import shutil
import time

import io, requests, zipfile

from resize import resize_textures

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--target_size", type=int, help="(Optional) Target size to resize the textures")
    args = parser.parse_args()
    if args.target_size is not None:
        print(f"Texture resize enabled: target size is {args.target_size}")

    project_root_dir = pathlib.Path.cwd()
    textures_dir = pathlib.Path(project_root_dir, "assets", "textures")
    with open(textures_dir / "source_textures.txt") as f:
        url_links = f.read().splitlines()
    downloads_dir = textures_dir / "downloads"
    downloads_dir.mkdir(exist_ok=True)

    print("Downloading...")
    start = time.time()
    for idx, url in enumerate(url_links):
        print(f"File {idx + 1} / {len(url_links)}: {url}")
        request = requests.get(url)
        zip_file = zipfile.ZipFile(io.BytesIO(request.content))
        zip_file.extractall(downloads_dir)
    end = time.time()
    print(f"Done! Approximated download time: {end - start} seconds")

    output_dir = textures_dir / "terrain"
    output_dir.mkdir(exist_ok=True)

    """
    In some cases, a texture can have different filenames for
    the same type of texture, such as 'albedo' or 'Base_Color'
    for albedo etc.
    """
    texture_alias = {
        "albedo": "albedo", 
        "Base_Color": "albedo",
        "ao": "ao", 
        "Ambient_Occlusion": "ao",
        "normal": "normal", 
        "Normal": "normal",
    }

    for image_path in downloads_dir.rglob("*.png"):
        # Move downloaded files to directory based on texture alias
        texture_type = next((alias for alias in list(texture_alias.keys()) if alias in str(image_path)), None)
        if texture_type is None:
            continue
        type_path = downloads_dir / texture_alias[texture_type]
        type_path.mkdir(exist_ok=True)
        image_path.rename(type_path / image_path.name)
    
    for alias in set(texture_alias.values()):
        image_dir = downloads_dir / alias
        final_dir = output_dir / alias
        if args.target_size is not None:
            resize_textures(image_dir, final_dir, "png", args.target_size)
        else:
            final_dir.mkdir(exist_ok=True)
            # Move files to final directory
            for image_path in image_dir.rglob("*.png"):
                image_path.rename(final_dir / image_path.stem)
    
    # Remove temporary folder
    shutil.rmtree(downloads_dir)

if __name__ == "__main__":
    main()