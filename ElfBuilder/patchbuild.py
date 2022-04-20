from email.policy import default
import os

"""builds a c file into a recon loadable .repatch file."""
def build_repatch(c_file, output_dest, delete_elf, opt):
    build_dest = os.path.splitext(c_file)[0] + ".elf"
    build_failed = os.system(f"clang --target=mipsr6el-linux-elf -nostdlib -static -fuse-ld=lld {c_file} -o {build_dest} -O{opt} -T linker.ld")

    if build_failed:
        print("Compile Error!!! (Bruh)")
        return False

    # convert string to hex for reason to parse the string
    def hexify(s):
        return "".join('{:02x}'.format(ord(c)) for c in s)

    elf_file = open(build_dest, "rb")
    program = hexify( elf_file.read().hex() )
    elf_file.close()    

    # max size of a string is 2 MB
    # divide by two since 2 hex chars is equivalent to a byte
    if len(program) // 2 > 2**21:
        print("ELF file is too big! Must be less than 1 MB")
        return False

    # repatch file content
    product_id = "se.kth.mipsemu"

    content = f"""<?xml version="1.0"?>
    <JukeboxPatch version="2.0"  deviceProductID="{product_id}"  deviceVersion="1.0d1" >
        <DeviceNameInEnglish>
            Mips Emulator
        </DeviceNameInEnglish>
        <Properties>
            <Object name="custom_properties" >
                <Value property="program"  type="string" >{program}</Value>
            </Object>
        </Properties>
    </JukeboxPatch>"""

    patchfile = open(output_dest, "w+")
    patchfile.write(content)
    patchfile.close()

    # remove elf file
    if delete_elf:
        os.remove(build_dest)

    return True


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser("patchbuild")
    parser.add_argument('cpath', help="c file path")
    parser.add_argument('dest', nargs='?', help="repatch file destination")
    parser.add_argument('--O', help="Optimization level", default='2')
    parser.add_argument('--rm', action='store_true', help="remove .elf file once finished.")

    args = parser.parse_args()
    dest = args.dest if args.dest else (os.path.splitext(args.cpath)[0] + ".repatch")

    exit(build_repatch(args.cpath, dest, args.rm, args.O))
    