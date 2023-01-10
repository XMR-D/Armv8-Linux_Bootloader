import os

Image_path = "thirdparty/Image"
pflash_path = "pflash.bin"

Image = open(Image_path, 'rb')
pflash = open(pflash_path, 'r+b')
Image_stats = os.stat(Image_path)

sent = 0
pflash.seek(91750400)
while (sent < Image_stats.st_size):
    print("progress : ", end="")
    print(sent, end="")
    print(" on ", end="")
    print(Image_stats.st_size, end='\r')
    data = Image.read(50000)
    pflash.write(data)
    sent += 50000
