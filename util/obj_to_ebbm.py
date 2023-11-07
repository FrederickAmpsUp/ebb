from sys import argv
import struct
import os

def obj_to_ebbm(in_fname: str, out_fname: str=None):
    basename = os.path.splitext(in_fname)[0]
    if out_fname is None:
        out_fname = basename + ".ebbm"

    vertices = []
    texcoords = []
    normals = []

    vert_indices = []
    tex_indices = []
    norm_indices = []

    final_vtable = []
    final_ftable = []

    with open(in_fname, 'r') as f:
        for line in f:
            if line.startswith('vt'):
                texcoord = [float(n) for n in line.split()[1:]]
                texcoords.append(texcoord)
            
            elif line.startswith('vn'):
                normal = [float(n) for n in line.split()[1:]]
                normals.append(normal)
            
            elif line.startswith('v '):
                vertex = [float(n) for n in line.split()[1:]]
                vertices.append(vertex)
            
            elif line.startswith('f'):
                indices = line.split()[1:]
                for index in indices:
                    vert_idx, tex_idx, norm_idx = index.split('/')
                    vert_indices.append(int(vert_idx) -1)
                    tex_indices.append(int(tex_idx)   -1)
                    norm_indices.append(int(norm_idx) -1)
    i = 0
    while i < len(vert_indices):
        final_vtable.append(
            b'v' + struct.pack('fff', *vertices[vert_indices[i]]) + struct.pack('ff', *texcoords[tex_indices[i]]) + struct.pack('fff', *normals[norm_indices[i]])
        )
        i += 1

    i = 0
    while i < len(vert_indices):
        final_ftable.append(
            b'f' + struct.pack('III',
                i+0,
                i+1,
                i+2)
        )
        i += 3

    with open(out_fname, 'wb') as f:
        for vert in final_vtable:
            f.write(vert)
        for face in final_ftable:
            f.write(face)

if __name__ == "__main__":
    if len(argv) < 2:
        print(f"Usage: {argv[0]} <filename>")
        exit(1)
    obj_to_ebbm(argv[1])