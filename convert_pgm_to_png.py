import requests

def convert_pgm_to_png(pgm_file_path):
    # Read the PGM file
    with open(pgm_file_path, 'rb') as file:
        pgm_data = file.read()

    # Prepare the request payload
    files = {
        'file': ('output.pgm', pgm_data),
    }

    # Make the API request
    response = requests.post('https://api.example.com/convert', files=files)

    # Check if the request was successful
    if response.status_code == 200:
        # Save the converted PNG file
        with open('final_output.png', 'wb') as file:
            file.write(response.content)
        print('Conversion successful. PNG file saved as image.png')
    else:
        print('Error converting the file. Status code:', response.status_code)


# Call the function with the path to your PGM file
convert_pgm_to_png('output.pgm')
