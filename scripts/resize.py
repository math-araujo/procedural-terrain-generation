import argparse
import pathlib
from PIL import Image
from typing import Union

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_dir", required=True, type=str, help="Directory where the textures to be resized reside. "
            "Relative to assets/textures/")
    parser.add_argument("--output_dir", default="output", type=str, help="Directory where the resized textures will be saved. "
            "Relative to assets/textures/")
    parser.add_argument("--target_size", default=256, type=int, help="Target size of the textures")
    parser.add_argument("--extension", default="png", type=str, help="Extension of the images")
    args = parser.parse_args()

    project_root_dir = pathlib.Path.cwd()
    textures_dir = pathlib.Path(project_root_dir, "assets", "textures")
    input_dir = pathlib.Path(textures_dir, args.input_dir)
    assert input_dir.is_dir(), f"Input directory is not a valid directory: {input_dir}"
    output_dir = pathlib.Path(textures_dir, args.output_dir)
    output_dir.mkdir(exist_ok=True)

    resize_textures(input_dir, output_dir, args.extension, args.target_size)

def resize_textures(input_dir: Union[str, pathlib.Path], output_dir: Union[str, pathlib.Path], file_extension: str, target_size: int):
    output_dir.mkdir(exist_ok=True)
    for image_path in input_dir.rglob(f"*.{file_extension}"):
        image_filename = image_path.name
        print(f"Current File: {image_filename}")
        image = Image.open(image_path)
        resized_image = image.resize((target_size, target_size), resample=Image.Resampling.LANCZOS)
        resized_dest = output_dir / f"{image_filename}"
        assert not resized_dest.is_file(), f"Image file {resized_dest} already exists in directory {output_dir}"
        
        if "ao" in str(image_path):
                resized_image = resized_image.convert("L")
        else:
                resized_image = resized_image.convert("RGB")

        resized_image.save(resized_dest)

if __name__ == "__main__":
    main()