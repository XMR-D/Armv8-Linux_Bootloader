from Crypto.Cipher import AES
import os

pflash = open("pflash.bin", "r+b")

pflash.seek(624)
data1 = pflash.read(640)

pflash.seek(13296)
data2 = pflash.read(256)

pflash.seek(13616)
data3 = pflash.read(256)

pflash.seek(14112)
data4 = pflash.read(640)

BLOCK_SIZE = 16 # Bytes

key = 'c14ed037716bcad2'
cipher = AES.new(key.encode('utf8'), AES.MODE_ECB)

data1 = cipher.encrypt(data1)
data2 = cipher.encrypt(data2)
data3 = cipher.encrypt(data3)
data4 = cipher.encrypt(data4)

decipher = AES.new(key.encode('utf8'), AES.MODE_ECB)
msg_dec = decipher.decrypt(data1)


pflash.seek(624)
data1 = pflash.write(data1)

pflash.seek(13296)
data2 = pflash.write(data2)

pflash.seek(13616)
data3 = pflash.write(data3)

pflash.seek(14112)
data4 = pflash.write(data4)