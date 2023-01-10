import os
import binascii
from Crypto.PublicKey import RSA
from Crypto.Hash import SHA256


#code from : https://www.matteomattei.com/how-to-calculate-the-crc32-of-a-file-in-python/
def CRC32_from_file(filename):
    buf = open(filename,'r+b').read()
    buf = (binascii.crc32(buf) & 0xFFFFFFFF)
    return "%08X" % buf

#code from me
path = 'thirdparty/Image'
Image = open(path, 'r+b')


Image_stats = os.stat(path)

Image = open(path, 'r+b')
Image.seek(65092)
Image.write((17224).to_bytes(2, "little")) #Custom Header spec
Image.write((Image_stats.st_size).to_bytes(4, "little")) #Size of the image
Image.write((0).to_bytes(4, "little"))
Image.close()

Image = open(path, 'r+b')
Image.seek(65102)
Image.write((0).to_bytes(384, "little"))
Image.seek(0)
SHA256val = SHA256.new()
SHA256val.update(Image.read()) #sha256 computing
SHA256val = int(SHA256val.hexdigest(), base=16)
Image.close()

keyPair = RSA.generate(bits=1024)

Signature = pow(SHA256val, keyPair.d, keyPair.n) #signature creation
R = pow(2, 2048, keyPair.n)

Image = open(path, 'r+b')

Image.seek(65102)
Image.write((Signature).to_bytes(128, "little"))
Image.write((keyPair.n).to_bytes(128, "little"))
Image.write((R).to_bytes(128, "little"))

Image.close()

CRC32val = int(CRC32_from_file(path), base=16) #crc32 computing
Image = open(path, 'r+b')
Image.seek(65098)
Image.write((CRC32val).to_bytes(4, "little"))