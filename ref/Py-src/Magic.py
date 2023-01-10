pflash = open("pflash.bin", "r+b")
pflash.seek(983040)
value = b'c14ed037716bcad2407716cbefe0463a1af8d7b3f5b92ad9a84f0ee864243791'
pflash.write(value)
pflash.close()