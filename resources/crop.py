from PIL import Image

def crop_cube_faces(input_image_path, output_dir, face_size):
    """
    Crops a flattened cube texture into individual face images.

    Parameters:
        input_image_path (str): Path to the input image containing the flattened cube texture.
        output_dir (str): Directory where the cropped face images will be saved.
        face_size (int): The size (width and height) of each face in pixels.
    """
    # Open the input image
    img = Image.open(input_image_path)

    # Define the positions of each face in a 4x3 cube map layout
    # Order: front, back, left, right, top, bottom
    face_positions = {
        "front": (1, 1),
        "back": (3, 1),
        "left": (0, 1),
        "right": (2, 1),
        "top": (1, 0),
        "bottom": (1, 2),
    }

    # Crop and save each face
    for face_name, (col, row) in face_positions.items():
        left = col * face_size
        top = row * face_size
        right = left + face_size
        bottom = top + face_size

        # Crop the face
        face_image = img.crop((left, top, right, bottom))

        # Save the cropped face
        output_path = f"{output_dir}/{face_name}.jpg"
        face_image.save(output_path)
        print(f"Saved {face_name} face to {output_path}")

# Example usage
if __name__ == "__main__":
    input_image_path = "skybox_texture.jpg"  # Path to your input image
    output_dir = "."  # Directory to save the cropped faces
    face_size = 1024  # Adjust based on the size of each face in your texture

    crop_cube_faces(input_image_path, output_dir, face_size)
