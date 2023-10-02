import numpy as np
import cv2

def add_salt_and_pepper_noise(image, noise_ratio):
    height, width = image.shape[:2]
    num_pixels = int(noise_ratio * height * width)

    # Generate random coordinates for noise pixels
    salt_coords = [np.random.randint(0, height, num_pixels), np.random.randint(0, width, num_pixels)]
    pepper_coords = [np.random.randint(0, height, num_pixels), np.random.randint(0, width, num_pixels)]

    # Add salt noise
    image[salt_coords] = 255

    # Add pepper noise
    image[pepper_coords] = 0

    return image

# Load the Lenna image
image = cv2.imread('lenna.png', cv2.IMREAD_GRAYSCALE)

# Resize the image to 512x512
resized_image = cv2.resize(image, (512, 512))

# Add salt and pepper noise to the image
noisy_image = add_salt_and_pepper_noise(resized_image, 0.05)  # Adjust the noise_ratio as desired

# Save the noisy image
cv2.imwrite('lenna_salt_pepper.png', noisy_image)
