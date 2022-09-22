import argparse
import pathlib
from PIL import Image

def resize_textures():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_dir", required=True, type=str, help="Directory where the textures to be resized reside. "
            "Relative to procedural-terrain-generation/data/textures/")
    parser.add_argument("--output_dir", default="output", type=str, help="Directory where the resized textures will be saved. "
            "Relative to procedural-terrain-generation/data/textures/")
    parser.add_argument("--target_size", default=256, type=int, help="Target size of the textures")
    parser.add_argument("--extension", default="png", type=str, help="Extension of the images")
    args = parser.parse_args()

    project_root_dir = pathlib.Path.cwd().parent
    textures_dir = pathlib.Path(project_root_dir, "data", "textures")
    input_dir = pathlib.Path(textures_dir, args.input_dir)
    assert input_dir.is_dir(), f"Input directory is not a valid directory: {input_dir}"
    output_dir = pathlib.Path(textures_dir, args.output_dir)
    output_dir.mkdir(exist_ok=True)

    for image_path in input_dir.glob(f"*.{args.extension}"):
        image_filename = image_path.stem
        print(f"Current File: {image_filename}")
        image = Image.open(image_path)
        resized_image = image.resize((args.target_size, args.target_size), resample=Image.Resampling.LANCZOS)
        resized_dest = output_dir / f"{image_filename}_{args.target_size}.{args.extension}"
        assert not resized_dest.is_file(), f"Image file {resized_dest} already exists in directory {output_dir}"
        resized_image.save(resized_dest)

if __name__ == "__main__":
    resize_textures()