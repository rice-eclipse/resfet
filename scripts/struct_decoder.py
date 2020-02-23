import struct

"""
File for converting binary logs on the Pi into human-readable logs. The format string
is hard-coded based on the format of the data written to the binary logs.
"""

format_string = "h6xQ"
read_filepath = "./"
write_filepath = "./"
filenames = ["LC_MAIN", "LC1", "LC2", "LC3", "PT_FEED", "PT_INJE", "PT_COMB", "TC1", "TC2", "TC3"]
cals = {"lc_main": (-0.0145 * 2.20462, 47.072 * 2.20462),
        "lc1": (-0.0044 * 2.20462, -2.5306 * 2.20462),
        "lc2": (-0.0043 * 2.20462, 16.128 * 2.20462),
        "lc3": (1, 0), "pt_feed": (-0.3012, 1116.9),
        "pt_inje": (-0.2996, 1111.1),
        "pt_comb": (-0.3, 1108.1),
        "tc1": (0.1611, -250),
        "tc2": (0.1611, -250),
        "tc3": (0.1611, -250)}


for i in range(len(filenames)):
    filename = filenames[i]
    # mtype = bytes([9 + i])                                                                              

    with open(read_filepath + filename + '.log', 'rb') as f, open(write_filepath + filename + '_Decoded.log', 'w') as p:

        f.seek(0, 2)
        file_size = f.tell()
        f.seek(0, 0)

        # print("File size", file_size, file_size / 260.0)

        for _ in range(int(file_size // 260)):
            data_line = f.read(260)
            data_bytes = bytes(data_line)

            # TODO do some verification with the header type
            header = data_bytes[:4]
            data = data_bytes[4:]
            # print("data size", len(data))
            for i in range(256 // 16):
                d, t = struct.unpack(format_string, bytes(data[i*16:i*16+16]))

                cal = cals[filename][0] * d + cals[filename][1]

                p.write(str(t) + " " + str(d) + " " + str(cal) + "\n")
