from PIL import Image
import io


def image_to_bytes(image: Image) -> tuple[bytes, tuple[int, int]]:
    # If the image has an alpha channel (RGBA), composite it onto white
    if image.mode in ("RGBA", "LA") or (image.mode == "P" and "transparency" in image.info):
        # Create a white background
        background = Image.new("RGB", image.size, (255, 255, 255))
        # Use the image's own alpha channel as the mask
        background.paste(image, mask=image.split()[-1]) 
        image = background

    bw_image = image.convert('1')

    raw_data = bw_image.tobytes()
    return raw_data, bw_image.size


def generate_c_array(
        img_bytes: bytes, 
        img_size:tuple[int, int],
        array_name:str = "gImage_2in13_Custom") -> str:
    
    hex_array = [f"0x{b:02x}" for b in img_bytes]
    header_comment = f"/*0X00,0X01,0X{img_size[0]:02X},0X00,0X{img_size[1]:02X},0X00,*/"

    # formatting
    lines = []
    for i in range(0, len(hex_array), 16):
        lines.append(",".join(hex_array[i:i+16]))
    body = ",\n".join(lines)

    c_code = f"""const unsigned char {array_name}[{len(hex_array)}] = {{ {header_comment}
{body}
}};"""

    return c_code

if __name__ == "__main__":
    img = Image.open("little_guy_full_screen.png")
    b, size = image_to_bytes(img)
    print(size)
    print(size[0])
    print(size[1])
    print(b)

    processed = Image.frombytes('1', size, b)
    processed.convert('1')
    processed.save("little_guy_full_screen.pbm", format="PPM")

    formatted = generate_c_array(b, size)
    print(formatted)


