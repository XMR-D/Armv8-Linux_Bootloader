import os

val = input("Enter value of char device: ")
print(val)

Dev_path = "/dev/pts/" + val
Image_path = "../thirdparty/Image"

dev = open(Dev_path, 'wb')
Image = open(Image_path, 'rb')

Image_stats = os.stat(Image_path)

sent = 0
while (sent < Image_stats.st_size):
    print("progress : ", end="")
    print(sent, end="")
    print(" on ", end="")
    print(Image_stats.st_size, end='\r')
    data = Image.read(500)
    dev.write(data)
    sent += 500