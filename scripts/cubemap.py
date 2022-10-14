import argparse
from PIL import Image
import pathlib
from typing import Union

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", required=True, type=str, help="Source path to the single cubemap image."
            "Relative to assets/textures/")
    parser.add_argument("--output_dir", default="cubemap", type=str, help="Destination directory where the 6 cubemap "
            " faces will be saved. Relative to assets/textures/")
    args = parser.parse_args()

    project_root_dir = pathlib.Path.cwd()
    textures_dir = pathlib.Path(project_root_dir, "assets", "textures")
    input_file = pathlib.Path(textures_dir, args.input)
    output_dir = textures_dir / args.output_dir

    crop_cubemap(input_file, output_dir)

def crop_cubemap(input_file: Union[str, pathlib.Path], output_dir: Union[str, pathlib.Path]):
    """
    Given a cubemap image, where the 6 cube faces are stored in a single image,
    crops and saves the cube faces as 6 separate images.
    """
    output_dir.mkdir(exist_ok=True)
    cubemap_image = Image.open(input_file).convert("RGB")
    width, height = cubemap_image.size
    horizontal_blocks = 4
    vertical_blocks = 3
    horizontal_offset = width / horizontal_blocks
    vertical_offset = height / vertical_blocks

    # Top
    cubemap_image.crop((horizontal_offset, 0, 2*horizontal_offset, vertical_offset)).save(output_dir / "top.png")
    
    # Bottom
    cubemap_image.crop((horizontal_offset, 2*vertical_offset, 2*horizontal_offset, 3*vertical_offset)).save(output_dir / "bottom.png")

    # Left, Back, Right, Front
    # NOTE: the order on LearnOpenGL's image (on the Cubemap tutorial) is Left,Front,Right,Back,
    # but it didn't work for my target cubemap images.
    for i, name in enumerate(("left", "back", "right", "front")):
        cubemap_image.crop((i * horizontal_offset, vertical_offset, (i + 1) * horizontal_offset, 2 * vertical_offset)).save(output_dir / f"{name}.png")

if __name__ == "__main__":
    main()